#ifndef SRCS_INCS_HTTPREQUEST_HPP_
#define SRCS_INCS_HTTPREQUEST_HPP_
#include <map>
#include <string>
#include <StringUtils.hpp>

class HttpRequest {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

		static const char	kCRLF_[];
		static const char	kWhitespace_[];

		std::string	method_;
		std::string	request_target_;
		std::string	http_version_;
		HeadersMap	headers_;
		std::string	body_;

		// Variable used by the Parse* methods
		// This is an index into the raw_request string
		// that keeps track of the start/end of the fields/delimiters.
		std::size_t	offset_;

	public:
		explicit	HttpRequest(const std::string &raw_request);
		std::string	GetMethod() const;
		std::string	GetRequestTarget() const;
		std::string	GetHttpVersion() const;
		std::string	GetHeaderValue(const std::string &header_name) const;
		std::string	GetBody() const;
		bool		HasHeader(const std::string &header_name) const;

	private:
		bool		ParseRawString_(const std::string &raw_request);
		bool		ParseRequestLine_(const std::string &raw_request);
		bool		ParseMethod_(const std::string &raw_request);
		bool		ParseRequestTarget_(const std::string &raw_request);
		bool		ParseHttpVersion_(const std::string &uri);
		bool		ParseHeaders_(const std::string &raw_request);
		std::string	ParseHeaderName_(const std::string &raw_request);
		std::string	ParseHeaderValue_(const std::string &raw_request);
		void		AddHeader_(const std::string &name, const std::string &val);
		bool		ParseBody_(const std::string &raw_request);
		bool		IsValidMethod_(const std::string &method) const;
		bool		IsValidRequestTarget_(const std::string &target) const;
		bool		IsValidHttpVersion_(const std::string &http_version) const;
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;
};

#endif  // SRCS_INCS_HTTPREQUEST_HPP_
