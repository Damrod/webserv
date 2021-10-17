#ifndef SRCS_INCS_HTTPREQUEST_HPP_
#define SRCS_INCS_HTTPREQUEST_HPP_
#include <map>
#include <string>
#include <StringUtils.hpp>
#include <CommonDefinitions.hpp>

class HttpRequest {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;
		typedef std::string							QueryName;
		typedef std::string							QueryValue;
		typedef	std::map<QueryName, QueryValue>		QueriesMap;

		static const char			kCRLF_[];
		static const char			kWhitespace_[];
		static const std::size_t	kPortMax;

		std::string	method_;
		std::string	request_target_;
		std::string	path_;
		QueriesMap	queries_;
		std::string	http_version_;
		HeadersMap	headers_;
		std::string	host_;
		std::size_t	port_;
		std::string	body_;

		// Variable used by the Parse* methods
		// This is an index into the raw_request string
		// that keeps track of the start/end of the fields/delimiters.
		std::size_t	offset_;

	public:
		explicit	HttpRequest(const std::string &raw_request);
		std::string	GetMethod() const;
		std::string	GetRequestTarget() const;
		std::string	GetPath() const;
		QueriesMap	GetQueries() const;
		std::string	GetQueryValue(const std::string &query_name) const;
		bool		HasQuery(const std::string &query_name) const;
		std::string	GetHttpVersion() const;
		HeadersMap	GetHeaders() const;
		std::string	GetHeaderValue(const std::string &header_name) const;
		std::string	GetHost() const;
		std::size_t	GetPort() const;
		std::string	GetBody() const;
		bool		HasHeader(const std::string &header_name) const;

	private:
		bool		ParseRawString_(const std::string &raw_request);
		bool		ParseRequestLine_(const std::string &raw_request);
		bool		ParseMethod_(const std::string &raw_request);
		bool		ParseRequestTarget_(const std::string &raw_request);
		bool		ParseQueryString_(const std::string &query_string);
		void		AddQuery_(const std::string &name, const std::string &val);
		bool		ParseHttpVersion_(const std::string &uri);
		bool		ParseHeaders_(const std::string &raw_request);
		std::string	ParseHeaderName_(const std::string &raw_request);
		std::string	ParseHeaderValue_(const std::string &raw_request);
		void		AddHeader_(const std::string &name, const std::string &val);
		bool		ParseHost_();
		bool		ParsePort_(const std::string &port_str);
		bool		ParseBody_(const std::string &raw_request);
		bool		IsValidPath_(const std::string &path) const;
		bool		IsValidHttpVersion_(const std::string &http_version) const;
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;
};

std::ostream&	operator<<(std::ostream &os, const HttpRequest &request);

#endif  // SRCS_INCS_HTTPREQUEST_HPP_
