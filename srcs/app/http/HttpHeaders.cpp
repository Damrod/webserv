#include <HttpHeaders.hpp>

void	HttpHeaders::ParseRawString(const std::string &raw_headers) {
	ParseHeaders_(raw_headers);
}

HttpHeaders::HeadersMap
HttpHeaders::GetHeaders() const {
	return headers_;
}

std::string	HttpHeaders::GetHeaderValue(const std::string &header_name) const {
	const std::string header_name_lc = ToLowerString(header_name);
	HeadersMap::const_iterator it = headers_.find(header_name_lc);
	if (it != headers_.end()) {
		return it->second;
	}
	return "";
}

bool	HttpHeaders::HasHeader(const std::string &header_name) const {
	return headers_.count(ToLowerString(header_name)) > 0;
}

void
HttpHeaders::AddHeader(const std::string &name, const std::string &value) {
	headers_.insert(std::make_pair(name, value));
}

void	HttpHeaders::Clear() {
	headers_.clear();
}

void	HttpHeaders::ParseHeaders_(const std::string &raw_headers) {
	std::size_t offset = 0;
	const std::size_t headers_end = raw_headers.size();

	while (offset < headers_end) {
		const std::size_t header_start = offset;
		const std::size_t header_end = raw_headers.find(Constants::kCRLF_, offset);
		if (header_end == std::string::npos) {
			throw std::runtime_error("Invalid header");
		}
		if (header_end - offset == 0) {
			break;
		}
		const std::string header =
					raw_headers.substr(header_start, header_end - header_start);
		ParseHeader_(header);
		offset = header_end + 2;
	}
}

void	HttpHeaders::ParseHeader_(const std::string &raw_header) {
	std::string name = ParseHeaderName_(raw_header);
	std::string value = ParseHeaderValue_(raw_header);
	AddHeader(name, value);
}

std::string	HttpHeaders::ParseHeaderName_(const std::string &raw_header) {
	const std::size_t name_end = raw_header.find(':');
	if (name_end == std::string::npos) {
		throw std::runtime_error("Invalid header");
	}
	std::string	name = raw_header.substr(0, name_end);
	if (!IsValidHeaderName_(name)) {
		throw std::runtime_error("Invalid header");
	}
	name = ToLowerString(name);
	return name;
}

std::string HttpHeaders::ParseHeaderValue_(const std::string &raw_header) {
	const std::size_t value_start = raw_header.find(':') + 1;
	std::string value = raw_header.substr(value_start);
	value = TrimString(value, Constants::kWhitespace_);
	if (!IsValidHeaderValue_(value)) {
		throw std::runtime_error("Invalid header");
	}
	return value;
}

// https://datatracker.ietf.org/doc/html/rfc7230#section-3.2.6
bool	HttpHeaders::IsValidHeaderName_(const std::string &name) const {
	const std::string valid_chars = "!#$%&'*+-.^_`|~"
								"0123456789"
								"abcdefghijklmnopqrstuvwxyz"
								"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return !name.empty() &&
		name.find_first_not_of(valid_chars) == std::string::npos;
}

bool	HttpHeaders::IsValidHeaderValue_(const std::string &value) const {
	return !value.empty() && ContainOnlyVisibleChars_(value);
}

bool	HttpHeaders::ContainOnlyVisibleChars_(const std::string &str) const {
	for (std::size_t i = 0; i < str.size(); ++i) {
		if (!std::isprint(static_cast<unsigned char>(str[i]))) {
			return false;
		}
	}
	return true;
}
