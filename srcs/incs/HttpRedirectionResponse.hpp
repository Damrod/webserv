#ifndef SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
#define SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_

#include <string>
#include <ctime>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>

class HttpRedirectionResponse: public HttpBaseResponse  {
	public:
		HttpRedirectionResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
};

#endif  // SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
