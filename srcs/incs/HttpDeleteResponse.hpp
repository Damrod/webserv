#ifndef SRCS_INCS_HTTPDELETERESPONSE_HPP_
#define SRCS_INCS_HTTPDELETERESPONSE_HPP_

#include <string>
#include <IResponse.hpp>
#include <HttpResponse.hpp>
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

class HttpDeleteResponse: public IResponse {
	public:
		HttpDeleteResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();

	private:
		void	CheckKeepAlive_();
		bool	HasAcceptedFormat_(const HttpRequest &request);
		void	AddCommonHeaders_(HttpResponse *response);
		std::string	CurrentDate_() const;

		RequestConfig *requestConfig_;
		HttpRequest *request_;
		bool	keep_alive_;
		std::size_t errCode;
		std::string raw_response_;
};

#endif  // SRCS_INCS_HTTPDELETERESPONSE_HPP_
