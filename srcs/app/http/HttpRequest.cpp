#include <HttpRequest.hpp>

const char HttpRequest::kCRLF_[] = "\r\n";
const char HttpRequest::kWhitespace_[] = " \t";

bool	HttpRequest::ParseRawString(const std::string &raw_request) {
	offset_ = 0;
	if (!ParseRequestLine_(raw_request))
		return false;
	if (!ParseHeaders_(raw_request))
		return false;
	if (!ParseBody_(raw_request))
		return false;
	return true;
}

std::string	HttpRequest::GetMethod() const {
	return method_;
}

std::string	HttpRequest::GetUri() const {
	return uri_;
}

std::string	HttpRequest::GetHttpVersion() const {
	return http_version_;
}

std::string	HttpRequest::GetHeaderValue(const std::string &header_name) const {
	const std::string			header_name_lc = ToLowerString(header_name);
	HeadersMap::const_iterator	map_it = headers_.find(header_name_lc);
	if (map_it != headers_.end())
		return map_it->second;
	return "";
}

std::string HttpRequest::GetBody() const {
	return body_;
}

bool	HttpRequest::HasHeader(const std::string &header_name) const {
	const std::string			header_name_lc = ToLowerString(header_name);
	HeadersMap::const_iterator	map_it = headers_.find(header_name_lc);
	return map_it != headers_.end();
}

bool	HttpRequest::ParseRequestLine_(const std::string &raw_request) {
	if (!ParseMethod_(raw_request))
		return false;
	if (!ParseUri_(raw_request))
		return false;
	if (!ParseHttpVersion_(raw_request))
		return false;
	return true;
}

bool	HttpRequest::ParseMethod_(const std::string &raw_request) {
	offset_ = raw_request.find(' ');
	if (offset_ == std::string::npos)
		return false;
	method_ = raw_request.substr(0, offset_);
	++offset_;
	return IsValidMethod_(method_);
}

bool	HttpRequest::IsValidMethod_(const std::string &method) const {
	const std::string	valid_http_methods[] = {"GET", "HEAD", "POST", "PUT",
												"DELETE", "CONNECT", "OPTIONS",
												"TRACE"};
	const std::size_t	len =
					sizeof(valid_http_methods) / sizeof(valid_http_methods[0]);
	for (std::size_t i = 0; i < len; ++i) {
		if (method == valid_http_methods[i])
			return true;
	}
	return false;
}

bool	HttpRequest::ParseUri_(const std::string &raw_request) {
	const std::size_t	uri_start = offset_;

	offset_ = raw_request.find(' ', uri_start);
	if (offset_ == std::string::npos)
		return false;
	uri_ = raw_request.substr(uri_start, offset_ - uri_start);
	++offset_;
	return IsValidUri_(uri_);
}

bool	HttpRequest::IsValidUri_(const std::string &uri) const {
	// TODO(gbudau) Look into how to validate URI
	(void)uri;
	return true;
}

bool	HttpRequest::ParseHttpVersion_(const std::string &raw_request) {
	const std::size_t	http_version_start = offset_;

	offset_ = raw_request.find(kCRLF_, http_version_start);
	if (offset_ == std::string::npos)
		return false;
	http_version_ = raw_request.substr(http_version_start,
										offset_ - http_version_start);
	offset_ += 2;
	return IsValidHttpVersion_(http_version_);
}

bool	HttpRequest::IsValidHttpVersion_(const std::string &http_version) const {
	return http_version == "HTTP/1.1";
}

bool	HttpRequest::ParseHeaders_(const std::string &raw_request) {
	while (offset_ < raw_request.length()) {
		const std::size_t header_end = raw_request.find(kCRLF_, offset_);
		if (header_end == std::string::npos)
			return false;
		if (header_end - offset_ == 0)
			break;
		HeaderName name = ParseHeaderName_(raw_request);
		if (name.empty())
			return false;
		HeaderValue value = ParseHeaderValue_(raw_request);
		if (value.empty())
			return false;
		AddHeader_(name, value);
		offset_ += 2;
	}
	offset_ += 2;
	return HasHeader("Host");
}

HttpRequest::HeaderName
HttpRequest::ParseHeaderName_(const std::string &raw_request) {
	const std::size_t name_start = offset_;

	offset_ = raw_request.find(':', name_start);
	if (offset_ == std::string::npos)
		return "";
	HeaderName	name = raw_request.substr(name_start, offset_ - name_start);
	if (!IsValidHeaderName_(name))
		return "";
	name = ToLowerString(name);
	++offset_;
	return name;
}

HttpRequest::HeaderValue
HttpRequest::ParseHeaderValue_(const std::string &raw_request) {
	const std::size_t	value_start = offset_;

	offset_ = raw_request.find(kCRLF_, offset_);
	if (offset_ == std::string::npos)
		return "";
	HeaderValue value = raw_request.substr(value_start, offset_ - value_start);
	value = TrimString(value, kWhitespace_);
	if (!IsValidHeaderValue_(value))
		return "";
	return value;
}

void	HttpRequest::AddHeader_(
		const HeaderName &name, const HeaderValue &value) {
	headers_.insert(std::make_pair(name, value));
}

// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
bool	HttpRequest::IsValidHeaderName_(const std::string &name) const {
	const std::string valid_chars = "!#$%&'*+-.^_`|~"
								"0123456789"
								"abcdefghijklmnopqrstuvwxyz"
								"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return !name.empty() &&
		name.find_first_not_of(valid_chars) == std::string::npos;
}

bool	HttpRequest::IsValidHeaderValue_(const std::string &value) const {
	return !value.empty() && ContainOnlyVisibleChars_(value);
}

bool	HttpRequest::ContainOnlyVisibleChars_(const std::string &str) const {
	for (std::size_t i = 0; i < str.size(); ++i) {
		if (!::isprint(str[i]))
			return false;
	}
	return true;
}

bool	HttpRequest::ParseBody_(const std::string &raw_request) {
	body_ = raw_request.substr(offset_);
	// TODO(gbudau) Review the Content-Length header definition
	// TODO(gbudau) Look into what we have to do if
	// the size of the body is different than the Content-Length
	return true;
}
