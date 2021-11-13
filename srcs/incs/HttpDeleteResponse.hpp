#ifndef SRCS_INCS_HTTPDELETERESPONSE_HPP_
#define SRCS_INCS_HTTPDELETERESPONSE_HPP_

#include <string>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <HttpBaseResponse.hpp>
#include <sys/stat.h>
#include <HttpRequest.hpp>
#include <ctime>
#include <RequestConfig.hpp>
#include <CGI.hpp>
#include <MimeTypes.hpp>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <iostream>

class HttpDeleteResponse: public HttpBaseResponse {
	public:
		HttpDeleteResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
};

#endif  // SRCS_INCS_HTTPDELETERESPONSE_HPP_
