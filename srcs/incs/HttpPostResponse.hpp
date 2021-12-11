#ifndef SRCS_INCS_HTTPPOSTRESPONSE_HPP_
#define SRCS_INCS_HTTPPOSTRESPONSE_HPP_

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <CGI.hpp>
#include <File.hpp>
#include <FormFile.hpp>
#include <MimeTypes.hpp>
#include <IResponse.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <RequestConfig.hpp>
#include <HttpBaseResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <Utils.hpp>


class HttpPostResponse: public HttpBaseResponse {
	public:
		HttpPostResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);

	private:
		bool	IsUploadEnabled_() const;
		bool	IsValidUploadPath_(const std::string &path) const;
		void	SetErrorRawResponse_(const int error_code);
		void	Upload_(const File &file);
		void	HandleCGI_(const File &file);
		std::string	ConstructFullPath_();
};

#endif  // SRCS_INCS_HTTPPOSTRESPONSE_HPP_
