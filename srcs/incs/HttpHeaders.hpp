#ifndef SRCS_INCS_HTTPHEADERS_HPP_
#define SRCS_INCS_HTTPHEADERS_HPP_
#include <map>
#include <string>

class HttpHeaders {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;

	public:
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

		explicit	HttpHeaders(const std::string &raw_headers);
		HeadersMap	GetHeaders() const;
		std::string	GetHeaderValue(const std::string &header_name) const;
		bool		HasHeader(const std::string &header_name) const;
		void		AddHeader(const std::string &name, const std::string &val);

	private:
		void		ParseHeaders_(const std::string &raw_headers);
		void		ParseHeader_(const std::string &raw_header);
		std::string	ParseHeaderName_(const std::string &raw_header);
		std::string	ParseHeaderValue_(const std::string &raw_header);
		bool		IsValidHeaderName_(const std::string &header_name) const;
		bool		IsValidHeaderValue_(const std::string &header_value) const;
		bool		ContainOnlyVisibleChars_(const std::string &str) const;

		HeadersMap	headers_;
};

#endif  // SRCS_INCS_HTTPHEADERS_HPP_
