#include <CgiHandler.hpp>

CgiHandler::CgiHandler(FDsets *fd_sets, int socket, const CgiInfo &cgi_info)
	: fd_sets_(fd_sets), socket_(socket), cgi_info_(cgi_info),
	cgi_complete_(false), headers_parsing_complete_(false), status_(200) {
	fd_sets_->addToReadSet(cgi_info_.cgi_output_fd);
}

CgiHandler::~CgiHandler() {
	fd_sets_->removeFd(socket_);
	fd_sets_->removeFd(cgi_info_.cgi_output_fd);
	SyscallWrap::closeWr(socket_ DEBUG_INFO);
	SyscallWrap::closeWr(cgi_info_.cgi_output_fd DEBUG_INFO);
}

ssize_t	CgiHandler::ReadCgiOutput() {
	char	buffer[4096];

	ssize_t nbytes = read(cgi_info_.cgi_output_fd, buffer, sizeof(buffer));
	if (nbytes > 0) {
		data_.append(&buffer[0], &buffer[nbytes]);
	} else if (nbytes == 0) {
		cgi_complete_ = true;
		fd_sets_->removeFd(cgi_info_.cgi_output_fd);
	}
	if (!headers_parsing_complete_) {
		TryParseHeaders_();
	}
	if (cgi_complete_ || headers_parsing_complete_) {
		fd_sets_->addToWriteSet(socket_);
	}
	return nbytes;
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
	return cgi_info_.cgi_output_fd;
}

bool	CgiHandler::IsComplete() const {
	return cgi_complete_ && data_.empty();
}

void	CgiHandler::TryParseHeaders_() {
	try {
		ParseHeaders_();
		if (cgi_complete_ && !headers_parsing_complete_) {
			throw std::runtime_error("Invalid header");
		}
	}
	catch (const std::exception &) {
		fd_sets_->removeFd(cgi_info_.cgi_output_fd);
		SetErrorResponse_(500);
		if (!cgi_complete_) {
			kill(cgi_info_.pid, SIGKILL);
		}
		cgi_complete_ = true;
	}
}

void	CgiHandler::ParseHeaders_() {
	const std::string delimiter =
							std::string(Constants::kCRLF_) + Constants::kCRLF_;
	const std::size_t headers_end = data_.find(delimiter);
	if (headers_end == std::string::npos) {
		return;
	}
	const std::string raw_headers = data_.substr(0, headers_end + 2);
	headers_.ParseRawString(raw_headers);
	ValidateHeaders_();
	PrependHeaders_();
	headers_parsing_complete_ = true;
	headers_.Clear();
}

void	CgiHandler::ValidateHeaders_() {
	if (!headers_.HasHeader("Content-Type")) {
		throw std::runtime_error("Invalid header");
	}
	if (headers_.HasHeader("Status")) {
		ParseStatus_(headers_.GetHeaderValue("Status"));
	}
}

void	CgiHandler::PrependHeaders_() {
	const std::string raw_response_headers = HttpResponse(
											status_,
											HttpResponse::HeadersMap(),
											"",
											false,
											true).RawContent();
	data_ = raw_response_headers + data_;
}

void	CgiHandler::ParseStatus_(const std::string &status_str) {
	const std::size_t delimiter = status_str.find(' ');
	if (delimiter == std::string::npos) {
		throw std::runtime_error("Invalid header");
	}
	const std::string status_code = status_str.substr(0, delimiter);
	const std::string valid_chars = "0123456789";
	if (status_code.empty() ||
			status_code.find_first_not_of(valid_chars) != std::string::npos) {
		throw std::runtime_error("Invalid header");
	}
	errno = 0;
	char *endptr;
	status_ = std::strtoul(status_code.c_str(), &endptr, 10);
	if (errno || *endptr != '\0' || !HttpStatusCodes::IsValid(status_)) {
		throw std::runtime_error("Invalid header");
	}
}

void	CgiHandler::SetErrorResponse_(const std::size_t &error_code) {
	const std::string error_page_path = GetErrorPagePath_(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		try {
			File file(error_page_path);

			Serve_(file, error_code);
		} catch (File::Error &e) {
			DefaultStatusResponse_(error_code);
		}
	}
}

std::string	CgiHandler::GetErrorPagePath_(const std::size_t &error_code) {
	if (cgi_info_.error_pages.count(error_code)) {
		return cgi_info_.root_path + cgi_info_.error_pages[error_code];
	}
	return "";
}

void	CgiHandler::DefaultStatusResponse_(const std::size_t &error_code) {
	HttpHeaders::HeadersMap headers;
	headers.insert(std::make_pair("Content-Type", "text/html"));

	SetResponse_(error_code, headers, "");
}

void	CgiHandler::Serve_(const File &file, const std::size_t &error_code) {
	HttpResponse::HeadersMap headers;
	std::string body;

	headers.insert(std::make_pair("Content-Type", file.GetMimeType()));
	body = file.GetContent();
	SetResponse_(error_code, headers, body);
}

void	CgiHandler::SetResponse_(const int &code,
								const HttpHeaders::HeadersMap &headers,
								const std::string &body) {
	data_ = HttpResponse(code,
						headers,
						body,
						false,
						false).RawContent();
}
