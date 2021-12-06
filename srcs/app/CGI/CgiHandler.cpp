#include <CgiHandler.hpp>

CgiHandler::CgiHandler(FDsets *fd_sets, int socket, int cgi_output)
	: fd_sets_(fd_sets), socket_(socket), cgi_output_(cgi_output),
	cgi_complete_(false), headers_parsing_complete_(false), status_(200) {
	fd_sets_->addToReadSet(cgi_output);
}

CgiHandler::~CgiHandler() {
	fd_sets_->removeFd(socket_);
	fd_sets_->removeFd(cgi_output_);
	SyscallWrap::closeWr(socket_ DEBUG_INFO);
	SyscallWrap::closeWr(cgi_output_ DEBUG_INFO);
}

ssize_t	CgiHandler::ReadCgiOutput() {
	char	buffer[4096];

	ssize_t nbytes = read(cgi_output_, buffer, sizeof(buffer));
	if (nbytes > 0) {
		data_.append(&buffer[0], &buffer[nbytes]);
	} else if (nbytes == 0) {
		cgi_complete_ = true;
		fd_sets_->removeFd(cgi_output_);
	}
	// TODO(gbudau) Refactor
	if (!headers_parsing_complete_) {
		try {
			TryParseHeaders_();
			if (cgi_complete_ && !headers_parsing_complete_) {
				throw std::runtime_error("Invalid header");
			}
		}
		catch (const std::exception &) {
			cgi_complete_ = true;
			fd_sets_->removeFd(cgi_output_);
			SetErrorResponse_();
			// TODO(gbudau) Kill the CGI process if is not complete
			//       we need the PID of the CGI process to do this
		}
	}
	if (cgi_complete_ || headers_parsing_complete_) {
		fd_sets_->addToWriteSet(socket_);
	}
	return nbytes;
}

void	CgiHandler::SetErrorResponse_() {
	// TODO(gbudau) Look into how to use HttpErrorResponse here
	//      we need to pass the Request and the RequestConfig to this class
	headers_.clear();
	headers_.insert(std::make_pair("Content-Type", "text/html"));
	data_ = HttpResponse(
											500,
											headers_,
											"",
											false,
											false).RawContent();
}

ssize_t	CgiHandler::SendCgiOutput() {
	ssize_t nbytes = send(socket_, data_.c_str(), data_.size(), 0);
	if (nbytes > 0) {
		data_.erase(0, nbytes);
	}
	if (data_.empty()) {
		fd_sets_->removeFd(socket_);
	}
	return nbytes;
}

int		CgiHandler::GetSocket() const {
	return socket_;
}

int		CgiHandler::GetCgiOutFd() const {
	return cgi_output_;
}

bool	CgiHandler::IsComplete() const {
	return cgi_complete_ && data_.empty();
}

void	CgiHandler::TryParseHeaders_() {
	const std::string delimiter =
							std::string(Constants::kCRLF_) + Constants::kCRLF_;
	const std::size_t headers_end = data_.find(delimiter);
	if (headers_end == std::string::npos) {
		return;
	}
	std::size_t offset = 0;
	while (offset < headers_end) {
		const std::size_t header_start = offset;
		const std::size_t header_end = data_.find(Constants::kCRLF_, offset);
		if (header_end == std::string::npos) {
			throw std::runtime_error("Invalid header");
		}
		if (header_end - offset == 0) {
			break;
		}
		const std::string header =
					data_.substr(header_start, header_end - header_start);
		ParseHeader_(header);
		offset = header_end + 2;
	}
	ValidateHeaders_();
	headers_parsing_complete_ = true;
	headers_.clear();
	const std::string raw_response_headers = HttpResponse(
											status_,
											headers_,
											"",
											false,
											true).RawContent();
	data_ = raw_response_headers + data_;
}

void	CgiHandler::ValidateHeaders_() {
	if (!HasHeader_("Content-Type")) {
		throw std::runtime_error("Invalid header");
	}
	if (HasHeader_("Status")) {
		ParseStatus_(GetHeaderValue_("Status"));
	}
}

void	CgiHandler::ParseHeader_(const std::string &header) {
	std::string name = ParseHeaderName_(header);
	std::string value = ParseHeaderValue_(header);
	if (name.empty() || value.empty()) {
		throw std::runtime_error("Invalid header");
	}
	headers_.insert(std::make_pair(name, value));
}

std::string CgiHandler::ParseHeaderName_(const std::string &header) const {
	const std::size_t name_end = header.find(':');
	if (name_end == std::string::npos) {
		return "";
	}
	std::string	name = header.substr(0, name_end);
	if (!IsValidHeaderName_(name)) {
		return "";
	}
	name = ToLowerString(name);
	return name;
}

std::string CgiHandler::ParseHeaderValue_(const std::string &header) const {
	const std::size_t value_start = header.find(':') + 1;
	std::string value = header.substr(value_start);
	value = TrimString(value, Constants::kWhitespace_);
	if (!IsValidHeaderValue_(value)) {
		return "";
	}
	return value;
}

// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
bool	CgiHandler::IsValidHeaderName_(const std::string &name) const {
	const std::string valid_chars = "!#$%&'*+-.^_`|~"
								"0123456789"
								"abcdefghijklmnopqrstuvwxyz"
								"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return !name.empty() &&
		name.find_first_not_of(valid_chars) == std::string::npos;
}

bool	CgiHandler::IsValidHeaderValue_(const std::string &value) const {
	return !value.empty() && ContainOnlyVisibleChars_(value);
}

bool	CgiHandler::ContainOnlyVisibleChars_(const std::string &str) const {
	for (std::size_t i = 0; i < str.size(); ++i) {
		if (!::isprint(str[i])) {
			return false;
		}
	}
	return true;
}

bool	CgiHandler::HasHeader_(const std::string &header_name) const {
	return headers_.count(ToLowerString(header_name)) > 0;
}

std::string
		CgiHandler::GetHeaderValue_(const std::string &header_name) const {
	const std::string header_name_lc = ToLowerString(header_name);
	HttpResponse::HeadersMap::const_iterator map_it =
												headers_.find(header_name_lc);
	if (map_it != headers_.end()) {
		return map_it->second;
	}
	return "";
}

void	CgiHandler::ParseStatus_(const std::string &status_str) {
	const std::string valid_chars = "0123456789";
	if (status_str.empty() ||
			status_str.find_first_not_of(valid_chars) != std::string::npos) {
		throw std::runtime_error("Invalid header");
	}
	errno = 0;
	char *endptr;
	status_ = std::strtoul(status_str.c_str(), &endptr, 10);
	if (errno || *endptr != '\0' || !HttpStatusCodes::IsValid(status_)) {
		throw std::runtime_error("Invalid header");
	}
}
