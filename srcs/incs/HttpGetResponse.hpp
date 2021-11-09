#ifndef SRCS_INCS_HTTPGETRESPONSE_HPP_
#define SRCS_INCS_HTTPGETRESPONSE_HPP_

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

class HttpGetResponse: public IResponse {
	public:
		HttpGetResponse(
			RequestConfig *requestConfig,
			HttpRequest *request);
		std::string content();

	private:
		// common
		void	CheckKeepAlive_();
		bool	HasAcceptedFormat_(const HttpRequest &request);
		void	AddCommonHeaders_(HttpResponse *response);
		std::string DefaultResponseBody_(const std::size_t status_code) const;
		void	PathError_();
		bool	IsRegularFile_(const std::string &path) const;
		bool	IsCGI_(const std::string &full_path) const;
		void	ExecuteCGI_(const HttpRequest &request,
							const std::string &full_path);
		void	ServeFile_(const std::string &file_path);
		void	RequestError_(const std::size_t error_code);
		void	DefaultStatusResponse_(const std::size_t status_code);
		std::string	PathExtension_(const std::string &path) const;
		std::string	GetMimeType_(const std::string &path) const;
		std::string	CurrentDate_() const;


		// unique
		bool	IsValidPath_(const std::string &path) const;
		void	MovedPermanently_(const HttpRequest &request);
		void	ListDirectory_(const std::string &request_path);
		bool	TryAddDirectoryContent_(std::stringstream *body,
										const std::string &full_path);
		bool	IsDirectory_(const std::string &path) const;

		RequestConfig *requestConfig_;
		HttpRequest *request_;
		bool	keep_alive_;
		std::size_t errCode;
		std::string raw_response_;
};

#endif  // SRCS_INCS_HTTPGETRESPONSE_HPP_
