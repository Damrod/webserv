#ifndef SRCS_INCS_HTTPERRORRESPONSE_HPP_
#define SRCS_INCS_HTTPERRORRESPONSE_HPP_

#include <string>
#include <ctime>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>

class HttpErrorResponse: public HttpBaseResponse {
	public:
		HttpErrorResponse(
			int error_code,
			RequestConfig *requestConfig,
			HttpRequest *request);
};

#endif  // SRCS_INCS_HTTPERRORRESPONSE_HPP_
