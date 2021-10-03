#ifndef SRCS_INCS_HTTPRESPONSE_HPP_
#define SRCS_INCS_HTTPRESPONSE_HPP_
#include <map>
#include <string>
#include <HttpStatusCodes.hpp>

class HttpResponse {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

	public:
		explicit	HttpResponse(std::size_t status_code);
		void		SetHttpVersion(const std::string &http_version);
		void		SetStatusCode(std::size_t status_code);
		void		SetReasonPhrase(const std::string &reason_phrase);
		void		AddHeader(const std::string &name, const std::string &val);
		void		SetBody(const std::string &body);
		std::string	GetHttpVersion() const;
		std::size_t	GetStatusCode() const;
		std::string	GetReasonPhrase() const;
		HeadersMap	GetHeaders() const;
		bool		HasHeader(const std::string &header_name) const;
		std::string	GetHeaderValue(const std::string &header_name) const;
		std::string	GetBody() const;
		std::string	CreateResponseString() const;

	private:
		static const char							kCRLF_[];

		std::string	http_version_;
		std::size_t	status_code_;
		std::string	reason_phrase_;
		HeadersMap	headers_;
		std::string	body_;
};

std::ostream&	operator<<(std::ostream &os, const HttpResponse &response);

#endif  // SRCS_INCS_HTTPRESPONSE_HPP_
