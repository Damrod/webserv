#ifndef SRCS_INCS_HTTPDELETERESPONSE_HPP_
#define SRCS_INCS_HTTPDELETERESPONSE_HPP_

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <CGI.hpp>
#include <IResponse.hpp>
#include <MimeTypes.hpp>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpErrorResponse.hpp>

class HttpDeleteResponse: public HttpBaseResponse {
	public:
		HttpDeleteResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);

	private:
		void	SetErrorRawResponse_(int error_code);
};

#endif  // SRCS_INCS_HTTPDELETERESPONSE_HPP_
