#ifndef SRCS_INCS_HTTPBASERESPONSE_HPP_
#define SRCS_INCS_HTTPBASERESPONSE_HPP_

#include <string>
#include <ctime>
#include <CGI.hpp>
#include <IResponse.hpp>
#include <AHttpResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <File.hpp>

class HttpBaseResponse {
	public:
		HttpBaseResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);

	protected:
		void	ExecuteCGI_(File file);
		void	Serve_(File file);
		void	SetRawResponse_(
								int code,
								AHttpResponse::HeadersMap headers,
								std::string body,
								bool keep_alive);

        int	keep_alive_;
        int	error_code_;
		std::string raw_response_;
		RequestConfig *request_config_;
		HttpRequest *request_;
};

#endif  // SRCS_INCS_HTTPBASERESPONSE_HPP_
