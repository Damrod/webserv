#ifndef SRCS_INCS_HTTPREQUEST_HPP_
#define SRCS_INCS_HTTPREQUEST_HPP_
#include <map>
#include <string>
#include <CommonDefinitions.hpp>
#include <IRequest.hpp>
#include <RequestState.hpp>
#include <Utils.hpp>

class HttpRequest : public IRequest {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;
		typedef std::string							QueryName;
		typedef std::string							QueryValue;
		typedef	std::map<QueryName, QueryValue>		QueriesMap;

	public:
		HttpRequest();
		~HttpRequest();
		std::string	GetMethod() const;
		std::string	GetRequestTarget() const;
		std::string	GetPath() const;
		std::string	GetDecodedPath() const;
		QueriesMap	GetQueries() const;
		std::string	GetQueryValue(const std::string &query_name) const;
		std::string	GetQueryString() const;
		std::string	GetHttpVersion() const;
		HeadersMap	GetHeaders() const;
		std::string	GetHeaderValue(const std::string &header_name) const;
		std::string	GetHost() const;
		std::size_t	GetPort() const;
		std::string	GetBody() const;
		RequestState::State	GetState() const;
		bool		HasHeader(const std::string &header_name) const;
		bool		HasQuery(const std::string &query_name) const;
		void		Reset();
		void		SetContent(const std::string &raw_request);
		std::size_t	ParsedOffset() const;
		bool		IsPartial() const;
		bool		IsComplete() const;

	private:
		HttpRequest(const HttpRequest &);
		HttpRequest &	operator=(const HttpRequest &);

		void		ParseRequestLine_(const std::string &raw_request);
		void		ParseMethod_(const std::string &request_line);
		void		ParseRequestTarget_(const std::string &request_line);
		void		ParseQueryString_(const std::string &query_string);
		void		AddQuery_(const std::string &name, const std::string &val);
		void		ParseHttpVersion_(const std::string &request_line);
		void		ParseHeaders_(const std::string &raw_request);
		bool		ParseHeader_(const std::string &header);
		std::string	ParseHeaderName_(const std::string &raw_request);
		std::string	ParseHeaderValue_(const std::string &raw_request);
		void		AddHeader_(const std::string &name, const std::string &val);
		void		ParseHost_();
		void		ParsePort_(const std::string &port_str);
		void		ParseContentLength_();
		void		ParseBody_(const std::string &raw_request);
		bool		IsValidPath_(const std::string &path) const;
		bool		IsValidDecodedPath_(const std::string &decoded_path) const;
		bool		IsValidHttpVersion_(const std::string &http_version) const;
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;

		enum ParseState_ {
			kParseRequestLine,
			kParseHeaders,
			kParseBody
		};

		static const char			        kCRLF_[];
		static const char			        kWhitespace_[];
		static const std::size_t	        kPortMax_;
		std::string                         method_;
		std::string                         request_target_;
		std::string                         path_;
		std::string                         decoded_path_;
		std::string                         http_version_;
		std::string                         host_;
		std::size_t                         port_;
		std::string                         body_;
		std::size_t                         content_length_;
		HeadersMap							headers_;
		QueriesMap							queries_;

		// This is an index into the raw_request string
		// that keeps track of the characters parsed so far.
		std::size_t	offset_;
		ParseState_	parse_state_;
		RequestState::State	state_;
};

std::ostream&	operator<<(std::ostream &os, const HttpRequest &request);

#endif  // SRCS_INCS_HTTPREQUEST_HPP_
