#ifndef SRCS_INCS_HTTPBASERESPONSE_HPP_
#define SRCS_INCS_HTTPBASERESPONSE_HPP_

#include <string>
#include <ctime>
#include <CGI.hpp>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <File.hpp>

class HttpBaseResponse: public IResponse {
	public:
		HttpBaseResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string	Content() const;
		bool	KeepAlive() const;
		bool	IsCgi() const;
		int		GetCgiOutputFd() const;

	protected:
		void	ExecuteCGI_(File file);
		void	Serve_(File file);
		void	DefaultStatusResponse_(int code);
		void	SetRawResponse_(
								int code,
								HttpResponse::HeadersMap headers,
								std::string body);

		int keep_alive_;
		int error_code_;
		std::string raw_response_;
		RequestConfig *request_config_;
		HttpRequest *request_;
		int cgi_output_fd_;
};

#endif  // SRCS_INCS_HTTPBASERESPONSE_HPP_
