#ifndef SRCS_INCS_HTTPDELETERESPONSE_HPP_
#define SRCS_INCS_HTTPDELETERESPONSE_HPP_

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <ftw.h>
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
		void	Delete_(File const &file);
		void	BuildResponse_(int return_status);
		void	SetErrorRawResponse_(int error_code);

		// needs to be static since nftw expects a
		// non-member __nftw_func_ type fuction
		static int	RemoveSingleFile_(
					const char *fpath,
					const struct stat *sb,
					int typeflag,
					struct FTW *ftwbuf);
};

#endif  // SRCS_INCS_HTTPDELETERESPONSE_HPP_
