#include <HttpRequest.hpp>
#include <stdexcept>
#include <cerrno>
#include <cstdlib>

const char			HttpRequest::kCRLF_[] = "\r\n";
const char			HttpRequest::kWhitespace_[] = " \t";
const std::size_t	HttpRequest::kPortMax = 65535;

HttpRequest::HttpRequest(const std::string &raw_request) {
	if (!ParseRawString_(raw_request))
		throw std::invalid_argument("[HttpRequest] Bad Request");
}

bool	HttpRequest::ParseRawString_(const std::string &raw_request) {
	offset_ = 0;
	if (!ParseRequestLine_(raw_request))
		return false;
	if (!ParseHeaders_(raw_request))
		return false;
	if (!ParseHost_())
		return false;
	if (!ParseBody_(raw_request))
		return false;
	return true;
}

std::string	HttpRequest::GetMethod() const {
	return method_;
}

std::string	HttpRequest::GetRequestTarget() const {
	return request_target_;
}

std::string	HttpRequest::GetPath() const {
	return path_;
}

std::map<std::string, std::string>	HttpRequest::GetQueries() const {
	return queries_;
}

std::string	HttpRequest::GetQueryValue(const std::string &query_name) const {
	QueriesMap::const_iterator	query_it = queries_.find(query_name);
	if (query_it != queries_.end())
		return query_it->second;
	return "";
}

bool	HttpRequest::HasQuery(const std::string &query_name) const {
	return queries_.count(query_name) > 0;
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

std::string	HttpRequest::GetHost() const {
	return host_;
}

std::size_t	HttpRequest::GetPort() const {
	return port_;
}

std::string HttpRequest::GetBody() const {
	return body_;
}

bool	HttpRequest::HasHeader(const std::string &header_name) const {
	return headers_.count(ToLowerString(header_name)) > 0;
}

bool	HttpRequest::ParseRequestLine_(const std::string &raw_request) {
	if (!ParseMethod_(raw_request))
		return false;
	if (!ParseRequestTarget_(raw_request))
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
	const std::string	valid_http_methods[] = {"GET", "POST", "DELETE"};
	const std::size_t	len =
					sizeof(valid_http_methods) / sizeof(valid_http_methods[0]);
	for (std::size_t i = 0; i < len; ++i) {
		if (method == valid_http_methods[i])
			return true;
	}
	return false;
}

bool	HttpRequest::ParseRequestTarget_(const std::string &raw_request) {
	const std::size_t	request_target_start = offset_;

	offset_ = raw_request.find(' ', request_target_start);
	if (offset_ == std::string::npos)
		return false;
	request_target_ = raw_request.substr(
			request_target_start, offset_ - request_target_start);
	++offset_;
	const std::size_t	query_delimiter = request_target_.find('?');
	if (query_delimiter == std::string::npos) {
		path_ = request_target_;
		return IsValidPath_(path_);
	}
	path_ = request_target_.substr(0, query_delimiter);
	std::string query_string = request_target_.substr(query_delimiter + 1);
	return IsValidPath_(path_) && ParseQueryString_(query_string);
}

bool	HttpRequest::ParseQueryString_(const std::string &query_string) {
	std::size_t	position = 0;

	while (position < query_string.size()) {
		const std::size_t	next_delimiter =
									query_string.find_first_of("&;", position);
		std::string	query;
		if (next_delimiter == std::string::npos)
			query = query_string.substr(position);
		else
			query = query_string.substr(position, next_delimiter - position);
		const std::size_t	pair_delimiter = query.find('=');
		const std::string	name = query.substr(0, pair_delimiter);
		if (name.empty())
			return false;
		std::string			value;
		if (pair_delimiter != std::string::npos)
			value = query.substr(pair_delimiter + 1);
		AddQuery_(name, value);
		if (next_delimiter == std::string::npos)
			break;
		position = next_delimiter + 1;
	}
	return true;
}

void	HttpRequest::AddQuery_(
					const std::string &name, const std::string &value) {
	queries_.insert(make_pair(name, value));
}

bool	HttpRequest::IsValidPath_(const std::string &path) const {
	return !path.empty() && path[0] == '/';
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
		std::string name = ParseHeaderName_(raw_request);
		if (name.empty())
			return false;
		std::string value = ParseHeaderValue_(raw_request);
		if (value.empty())
			return false;
		AddHeader_(name, value);
		offset_ += 2;
	}
	offset_ += 2;
	return true;
}

std::string HttpRequest::ParseHeaderName_(const std::string &raw_request) {
	const std::size_t name_start = offset_;

	offset_ = raw_request.find(':', name_start);
	if (offset_ == std::string::npos)
		return "";
	std::string	name = raw_request.substr(name_start, offset_ - name_start);
	if (!IsValidHeaderName_(name))
		return "";
	name = ToLowerString(name);
	++offset_;
	return name;
}

std::string HttpRequest::ParseHeaderValue_(const std::string &raw_request) {
	const std::size_t	value_start = offset_;

	offset_ = raw_request.find(kCRLF_, offset_);
	if (offset_ == std::string::npos)
		return "";
	std::string value = raw_request.substr(value_start, offset_ - value_start);
	value = TrimString(value, kWhitespace_);
	if (!IsValidHeaderValue_(value))
		return "";
	return value;
}

void	HttpRequest::AddHeader_(
		const std::string &name, const std::string &value) {
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

// Parse the Host header into a host and optional port number
// Host = uri-host [ ":" port ]
bool	HttpRequest::ParseHost_() {
	std::string host = GetHeaderValue("Host");
	if (host.empty())
		return false;
	std::size_t port_delimiter = host.find(':');
	if (port_delimiter == std::string::npos) {
		host_ = host;
		port_ = 80;
		return true;
	}
	host_ = host.substr(0, port_delimiter);
	std::string port_str = host.substr(port_delimiter + 1);
	if (host_.empty() || port_str.empty())
		return false;
	errno = 0;
	char *endptr;
	port_ = std::strtoul(port_str.c_str(), &endptr, 10);
	return !errno && *endptr == '\0' && port_ <= kPortMax;
}

bool	HttpRequest::ParseBody_(const std::string &raw_request) {
	body_ = raw_request.substr(offset_);
	if (body_.empty() && !HasHeader("Content-Length"))
		return true;
	errno = 0;
	char *endptr;
	std::size_t content_length = std::strtoul(
			GetHeaderValue("Content-Length").c_str(), &endptr, 10);
	return !errno && *endptr == '\0' && content_length == body_.size();
}
