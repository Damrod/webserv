#include <AHttpResponse.hpp>
#include <sstream>
#include <stdexcept>
#include <HttpStatusCodes.hpp>
#include <StringUtils.hpp>

const char AHttpResponse::kCRLF_[] = "\r\n";

AHttpResponse::AHttpResponse(
			std::size_t status_code,
			std::map<HeaderName, HeaderValue> headers,
			std::string	body,
			bool	keep_alive,
			bool	default_body) :
			status_code_(status_code),
			headers_(headers),
			body_(body),
			keep_alive_(keep_alive),
			default_body_(default_body),
			http_version_("HTTP/1.1"),
			reason_phrase_(HttpStatusCodes::GetReasonPhrase(status_code))
{
	if (!HttpStatusCodes::IsValid(status_code)) {
		throw std::invalid_argument("[AHttpResponse] Invalid Status");
	}
	AddCommonHeaders_();
	AddContentLength_();
	if (default_body_) {
		AddDefaultResponseBody_();
	}
}

AHttpResponse::~AHttpResponse() {}

std::string	AHttpResponse::RawContent() const {
	std::stringstream	ss;

	ss << http_version_ << ' ' << status_code_ << ' ' << reason_phrase_ <<
		kCRLF_;
	HeadersMap::const_iterator	it = headers_.begin();
	HeadersMap::const_iterator	ite = headers_.end();
	while (it != ite) {
		ss << it->first << ": " << it->second << kCRLF_;
		++it;
	}
	ss << kCRLF_;
	ss << body_;
	return ss.str();
}

void	AHttpResponse::AddContentLength_() {
	std::stringstream	ss;
	ss << body_.size();
	headers_["content-length"] = ss.str();
}

void	AHttpResponse::AddCommonHeaders_() {
	AddHeader_("Server", "webserv");
	if (keep_alive_) {
		AddHeader_("Connection", "keep-alive");
	} else {
		AddHeader_("Connection", "close");
	}
	AddHeader_("Date", CurrentDate_());
}

std::string	AHttpResponse::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

std::string	AHttpResponse::AddDefaultResponseBody_() {
	std::stringstream	ss;
	ss << status_code_ << " " << HttpStatusCodes::GetReasonPhrase(status_code_);
	const std::string	message = ss.str();
	body_ = "<html>\n"
		"<head><title>" + message + "</title></head>\n"
		"<body>\n"
		"<center><h1>" + message + "</h1></center>\n"
		"<hr><center>webserv</center>\n"
		"</body>\n"
		"</html>\n";
}

std::ostream&	operator<<(std::ostream &os, const AHttpResponse &response) {
	os << response.RawContent();
	return os;
}
