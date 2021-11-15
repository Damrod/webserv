#ifndef SRCS_INCS_HTTPRESPONSE_HPP_
#define SRCS_INCS_HTTPRESPONSE_HPP_

#include <ctime>
#include <map>
#include <utility>
#include <string>
#include <HttpStatusCodes.hpp>

class HttpResponse {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;

	public:
		explicit	HttpResponse(
						std::size_t status_code,
						std::map<HeaderName, HeaderValue> headers,
						std::string	body,
						bool	keep_alive);
		virtual std::string	RawContent() const;

		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

	private:
		HttpResponse();
		HttpResponse(const HttpResponse &);
		HttpResponse &	operator=(const HttpResponse &);
		void	AddHeader_(const std::string &name, const std::string &val);
		void	AddDefaultResponseBody_();
		void	AddCommonHeaders_();
		void	AddContentLength_();
		std::string	CurrentDate_() const;

		static const char	kCRLF_[];
		std::size_t	status_code_;
		HeadersMap	headers_;
		std::string	body_;
		bool	keep_alive_;
		std::string	http_version_;
		std::string	reason_phrase_;
};

std::ostream&	operator<<(std::ostream &os, const HttpResponse &response);

#endif  // SRCS_INCS_HTTPRESPONSE_HPP_
