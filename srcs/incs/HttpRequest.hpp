#ifndef SRCS_INCS_HTTPREQUEST_HPP_
#define SRCS_INCS_HTTPREQUEST_HPP_
#include <map>
#include <string>

class HttpRequest {
	private:
		// Map containing pairs of header_name:header_value
		typedef	std::map<std::string, std::string> HeadersMap;

		static const char	CRLF[];
		static const char	WSP[];

		std::string	method_;
		std::string	uri_;
		std::string	http_version_;
		HeadersMap	headers_;
		std::string	body_;
		std::size_t	offset_;

	public:
		bool		ParseRawString(const std::string &raw_request);
		std::string	GetMethod() const;
		std::string	GetUri() const;
		std::string	GetHttpVersion() const;
		std::string GetHeaderValue(const std::string &header_name) const;
		std::string	GetBody() const;
		bool		HasHeader(const std::string &header_name) const;

	private:
		bool		ParseRequestLine(const std::string &raw_request);
		bool		ParseMethod(const std::string &raw_request);
		bool		ParseUri(const std::string &raw_request);
		bool		ParseHttpVersion(const std::string &uri);
		bool		ParseHeaders(const std::string &raw_request);
		bool		ParseBody(const std::string &raw_request);
		bool		IsValidMethod(const std::string &method) const;
		bool		IsValidUri(const std::string &uri) const;
		bool		IsValidHttpVersion(const std::string &http_version) const;
		bool		IsValidHeaderName(const std::string &header_name) const;
		bool		IsValidHeaderValue(const std::string &header_value) const;
		std::string	TrimWhitespace(const std::string &header_value) const;
		std::string	ToLowerCaseString(const std::string &str) const;
		bool		ContainOnlyAsciiCharacters(const std::string &str) const;
};

#endif  // SRCS_INCS_HTTPREQUEST_HPP_
