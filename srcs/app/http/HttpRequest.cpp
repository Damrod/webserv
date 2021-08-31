#include <HttpRequest.hpp>
#include <algorithm>
#include <cctype>

const char HttpRequest::CRLF[] = "\r\n";
const char HttpRequest::WSP[] = " \t";

bool	HttpRequest::ParseRawString(const std::string &raw_request) {
	offset_ = 0;
	if (!ParseRequestLine(raw_request))
		return false;
	if (!ParseHeaders(raw_request))
		return false;
	if (!ParseBody(raw_request))
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
	std::string					header_name_lc = ToLowerCaseString(header_name);
	HeadersMap::const_iterator	map_it = headers_.find(header_name_lc);
	if (map_it != headers_.end())
		return map_it->second;
	return "";
}

std::string HttpRequest::GetBody() const {
	return body_;
}

bool	HttpRequest::HasHeader(const std::string &header_name) const {
	std::string					header_name_lc = ToLowerCaseString(header_name);
	HeadersMap::const_iterator	map_it = headers_.find(header_name_lc);
	return map_it != headers_.end();
}

std::string	HttpRequest::ToLowerCaseString(const std::string &str) const {
	std::string lowercase_str = str;
	std::transform(lowercase_str.begin(), lowercase_str.end(),
			lowercase_str.begin(), ::tolower);
	return lowercase_str;
}

bool	HttpRequest::ParseRequestLine(const std::string &raw_request) {
	if (!ParseMethod(raw_request))
		return false;
	if (!ParseUri(raw_request))
		return false;
	if (!ParseHttpVersion(raw_request))
		return false;
	return true;
}

bool	HttpRequest::ParseMethod(const std::string &raw_request) {
	offset_ = raw_request.find(' ');
	if (offset_ == std::string::npos)
		return false;
	method_ = raw_request.substr(0, offset_);
	++offset_;
	return IsValidMethod(method_);
}

bool	HttpRequest::IsValidMethod(const std::string &method) const {
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

bool	HttpRequest::ParseUri(const std::string &raw_request) {
	const std::size_t	uri_start = offset_;

	offset_ = raw_request.find(' ', uri_start);
	if (offset_ == std::string::npos)
		return false;
	uri_ = raw_request.substr(uri_start, offset_ - uri_start);
	++offset_;
	return IsValidUri(uri_);
}

bool	HttpRequest::IsValidUri(const std::string &uri) const {
	// TODO(gbudau) Look into how to validate URI
	(void)uri;
	return true;
}

bool	HttpRequest::ParseHttpVersion(const std::string &raw_request) {
	const std::size_t	http_version_start = offset_;

	offset_ = raw_request.find(CRLF, http_version_start);
	if (offset_ == std::string::npos)
		return false;
	http_version_ = raw_request.substr(http_version_start,
										offset_ - http_version_start);
	offset_ += 2;
	return IsValidHttpVersion(http_version_);
}

bool	HttpRequest::IsValidHttpVersion(const std::string &http_version) const {
	return http_version == "HTTP/1.1";
}

bool	HttpRequest::ParseHeaders(const std::string &raw_request) {
	while (offset_ < raw_request.length()) {
		const std::size_t header_name_start = offset_;
		offset_ = raw_request.find(CRLF, offset_);
		if (offset_ == std::string::npos)
			return false;
		if (offset_ - header_name_start == 0)
			break;
		std::size_t header_delimiter = raw_request.find(':', header_name_start);
		if (header_delimiter == std::string::npos)
			return false;
		std::string	header_name = raw_request.substr(header_name_start,
										header_delimiter -  header_name_start);
		if (!IsValidHeaderName(header_name))
			return false;
		header_name = ToLowerCaseString(header_name);
		++header_delimiter;
		std::string header_value = raw_request.substr(header_delimiter,
										offset_ - header_delimiter);
		header_value = TrimWhitespace(header_value);
		if (!IsValidHeaderValue(header_value))
			return false;
		headers_.insert(std::make_pair(header_name, header_value));
		offset_ += 2;
	}
	offset_ += 2;
	return HasHeader("Host");
}

// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
bool	HttpRequest::IsValidHeaderName(const std::string &header_name) const {
	std::string valid_chars = "!#$%&'*+-.^_`|~"
								"0123456789"
								"abcdefghijklmnopqrstuvwxyz"
								"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return !header_name.empty() &&
		header_name.find_first_not_of(valid_chars) == std::string::npos;
}

// TODO(gbudau) Look into which characters are valid for header values
bool	HttpRequest::IsValidHeaderValue(const std::string &header_value) const {
	return !header_value.empty() && ContainOnlyAsciiCharacters(header_value);
}

bool	HttpRequest::ContainOnlyAsciiCharacters(const std::string &str) const {
	for (std::size_t i = 0; i < str.size(); ++i) {
		if (static_cast<unsigned char>(str[i]) > 127)
			return false;
	}
	return true;
}

std::string	HttpRequest::TrimWhitespace(const std::string &header_value) const {
	const std::size_t	start_position = header_value.find_first_not_of(WSP);

	if (start_position == std::string::npos)
		return "";
	const std::size_t	end_position = header_value.find_last_not_of(WSP) + 1;
	return header_value.substr(start_position, end_position - start_position);
}

bool	HttpRequest::ParseBody(const std::string &raw_request) {
	body_ = raw_request.substr(offset_);
	// TODO(gbudau) Check if there is a Content-Length header thats valid
	// And the offset is atleast the size of the Content-Length
	// TODO(gbudau) Look into what we have to do if
	// the size of the body is greater than the Content-Length
	return true;
}
