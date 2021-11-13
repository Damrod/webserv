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

	public:
		explicit	AHttpResponse(
						std::size_t status_code,
						std::map<HeaderName, HeaderValue> headers,
						std::string	body,
						bool	keep_alive);
					~AHttpResponse();
		virtual std::string	RawContent() const;

		typedef	std::map<HeaderName, HeaderValue>	HeadersMap;

	private:
		AHttpResponse();
		AHttpResponse(const AHttpResponse &);
		AHttpResponse &	operator=(const AHttpResponse &);
		void	AddHeader_(const std::string &name, const std::string &val);
		std::string	AddDefaultResponseBody_();
		void	AddCommonHeaders_();
		void	AddContentLength_();
		std::string	CurrentDate_() const;

		static const char	kCRLF_[];
		std::string	http_version_;
		std::size_t	status_code_;
		std::string	reason_phrase_;
		HeadersMap	headers_;
		std::string	body_;
		bool	keep_alive_;
};

std::ostream&	operator<<(std::ostream &os, const AHttpResponse &response);

#endif  // SRCS_INCS_AHTTPRESPONSE_HPP_
