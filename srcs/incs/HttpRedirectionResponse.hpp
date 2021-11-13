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

class HttpRedirectionResponse: public IResponse, HttpBaseResponse  {
	public:
		HttpRedirectionResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();
};

#endif  // SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
