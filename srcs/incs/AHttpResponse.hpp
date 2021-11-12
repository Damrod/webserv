#ifndef SRCS_INCS_AHTTPRESPONSE_HPP_
#define SRCS_INCS_AHTTPRESPONSE_HPP_

#include <ctime>
#include <map>
#include <string>
#include <HttpStatusCodes.hpp>

class AHttpResponse {
	private:
		typedef std::string							HeaderName;
		typedef std::string							HeaderValue;
		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

	public:
		explicit	AHttpResponse(
						std::size_t status_code,
						std::map<HeaderName, HeaderValue> headers,
						std::string	body,
						bool	keep_alive,
						bool	default_body);
					~AHttpResponse();
		std::string	RawContent() const;

	private:
		AHttpResponse();
		AHttpResponse(const AHttpResponse &);
		void	AddHeader_(const std::string &name, const std::string &val);
		std::string	AddDefaultResponseBody_();
		void	AddCommonHeaders_();
		void	AddContentLength_();
		std::string	CurrentDate_() const;
		AHttpResponse &	operator=(const AHttpResponse &);

		static const char							kCRLF_[];

		std::string	http_version_;
		std::size_t	status_code_;
		std::string	reason_phrase_;
		HeadersMap	headers_;
		std::string	body_;
		bool	keep_alive_;
		bool	default_body_;

	// posible lógica desplazada
	// std::string	CurrentDate_() const;
	// void	AddCommonHeaders_(HttpResponse *response);
	// std::string DefaultResponseBody_(const std::size_t status_code) const;

};

std::ostream&	operator<<(std::ostream &os, const AHttpResponse &response);

#endif  // SRCS_INCS_AHTTPRESPONSE_HPP_
