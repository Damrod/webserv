#include <HttpResponse.hpp>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <StringUtils.hpp>

const char HttpResponse::kCRLF_[] = "\r\n";

const std::map<std::size_t, std::string>
HttpResponse::kResponseStatusMap = CreateResponseStatusMap();

const std::map<std::size_t, std::string>
HttpResponse::CreateResponseStatusMap() {
	ResponseStatusMap rs;

	rs.insert(std::make_pair(100, "Continue"));
	rs.insert(std::make_pair(101, "Switching Protocols"));
	rs.insert(std::make_pair(200, "OK"));
	rs.insert(std::make_pair(201, "Created"));
	rs.insert(std::make_pair(202, "Accepted"));
	rs.insert(std::make_pair(203, "Non-Authoritative Information"));
	rs.insert(std::make_pair(204, "No Content"));
	rs.insert(std::make_pair(205, "Reset Content"));
	rs.insert(std::make_pair(206, "Partial Content"));
	rs.insert(std::make_pair(300, "Multiple Choices"));
	rs.insert(std::make_pair(301, "Moved Permanently"));
	rs.insert(std::make_pair(302, "Found"));
	rs.insert(std::make_pair(303, "See Other"));
	rs.insert(std::make_pair(304, "Not Modified"));
	rs.insert(std::make_pair(305, "Use Proxy"));
	rs.insert(std::make_pair(307, "Temporary Redirect"));
	rs.insert(std::make_pair(400, "Bad Request"));
	rs.insert(std::make_pair(401, "Unauthorized"));
	rs.insert(std::make_pair(402, "Payment Required"));
	rs.insert(std::make_pair(403, "Forbidden"));
	rs.insert(std::make_pair(404, "Not Found"));
	rs.insert(std::make_pair(405, "Method Not Allowed"));
	rs.insert(std::make_pair(406, "Not Acceptable"));
	rs.insert(std::make_pair(407, "Proxy Authentication Required"));
	rs.insert(std::make_pair(408, "Request Timeout"));
	rs.insert(std::make_pair(409, "Conflict"));
	rs.insert(std::make_pair(410, "Gone"));
	rs.insert(std::make_pair(411, "Length Required"));
	rs.insert(std::make_pair(412, "Precondition Failed"));
	rs.insert(std::make_pair(413, "Payload Too Large"));
	rs.insert(std::make_pair(414, "URI Too Long"));
	rs.insert(std::make_pair(415, "Unsupported Media Type"));
	rs.insert(std::make_pair(416, "Range Not Satisfiable"));
	rs.insert(std::make_pair(417, "Expectation Failed"));
	rs.insert(std::make_pair(426, "Upgrade Required"));
	rs.insert(std::make_pair(500, "Internal Server Error"));
	rs.insert(std::make_pair(501, "Not Implemented"));
	rs.insert(std::make_pair(502, "Bad Gateway"));
	rs.insert(std::make_pair(503, "Service Unavailable"));
	rs.insert(std::make_pair(504, "Gateway Timeout"));
	rs.insert(std::make_pair(505, "HTTP Version Not Supported"));
	return rs;
}

HttpResponse::HttpResponse(std::size_t status_code) {
	ResponseStatusMap::const_iterator it = kResponseStatusMap.find(status_code);
	if (it == kResponseStatusMap.end())
		throw std::invalid_argument("[HttpResponse] Invalid Status");
	http_version_ = "HTTP/1.1";
	status_code_ = it->first;
	reason_phrase_ = it->second;
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
