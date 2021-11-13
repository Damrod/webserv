#ifndef SRCS_INCS_HTTPERRORRESPONSE_HPP_
#define SRCS_INCS_HTTPERRORRESPONSE_HPP_

#include <string>
#include <ctime>
#include <IResponse.hpp>
#include <AHttpResponse.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>

class HttpErrorResponse: public IResponse, HttpBaseResponse {
	public:
		HttpErrorResponse(
			int error_code,
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();

	private:
		int error_code_;
		RequestConfig *requestConfig_;
		HttpRequest *request_;
		std::string raw_response_;
		int keep_alive_;
};

#endif  // SRCS_INCS_HTTPERRORRESPONSE_HPP_
