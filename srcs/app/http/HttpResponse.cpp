#include <HttpResponse.hpp>
#include <sstream>
#include <stdexcept>
#include <HttpStatusCodes.hpp>
#include <StringUtils.hpp>

const char HttpResponse::kCRLF_[] = "\r\n";

HttpResponse::HttpResponse(std::size_t status_code) {
	const HttpStatusCodes response_codes;
	if (!response_codes.IsValid(status_code))
		throw std::invalid_argument("[HttpResponse] Invalid Status");
	http_version_ = "HTTP/1.1";
	status_code_ = status_code;
	reason_phrase_ = response_codes.GetReasonPhrase(status_code);
}

void	HttpResponse::SetHttpVersion(const std::string &http_version) {
	http_version_ = http_version;
}

void	HttpResponse::SetStatusCode(std::size_t status_code) {
	status_code_ = status_code;
}

void	HttpResponse::SetReasonPhrase(const std::string &reason_phrase) {
	reason_phrase_ = reason_phrase;
}

void	HttpResponse::AddHeader(const std::string &name,
								const std::string &value) {
	headers_.insert(std::make_pair(ToLowerString(name), value));
}

void	HttpResponse::SetBody(const std::string &body) {
	body_ = body;
	std::stringstream	ss;
	ss << body.size();
	headers_["content-length"] = ss.str();
}

std::string	HttpResponse::GetHttpVersion() const {
	return http_version_;
}

std::size_t	HttpResponse::GetStatusCode() const {
	return status_code_;
}

std::string HttpResponse::GetReasonPhrase() const {
	return reason_phrase_;
}

std::map<std::string, std::string>	HttpResponse::GetHeaders() const {
	return headers_;
}

bool	HttpResponse::HasHeader(const std::string &header_name) const {
	return headers_.count(ToLowerString(header_name)) > 0;
}

std::string	HttpResponse::GetHeaderValue(const std::string &header_name) const {
	HeadersMap::const_iterator	header_it =
									headers_.find(ToLowerString(header_name));
	if (header_it != headers_.end())
		return header_it->second;
	return "";
}

std::string	HttpResponse::GetBody() const {
	return body_;
}

std::string	HttpResponse::CreateResponseString() const {
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

std::ostream&	operator<<(std::ostream &os, const HttpResponse &response) {
	os << response.CreateResponseString();
	return os;
}
