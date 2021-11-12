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
		// Internal
		void	CheckKeepAlive_();
		bool	HasAcceptedFormat_(const HttpRequest &request);

		// Default - base
		void	AddCommonHeaders_(HttpResponse *response);
		std::string DefaultResponseBody_(const std::size_t status_code) const;
		std::string	CurrentDate_() const;

		// CGI
		bool	IsCGI_(const std::string &full_path) const; // PathExtension_

		// extrapolable
		void	ExecuteCGI_(const HttpRequest &request,
							const std::string &full_path); // AddCommonHeaders_, PathExtension_, RequestError_

		// Error
		void	RequestError_(const std::size_t error_code); // DefaultStatusReponse_, ServeFile_
		void	DefaultStatusResponse_(const std::size_t status_code); //AddCommonHeaders_
		void	PathError_(); // RequestError_

		//  Files
		void	ServeFile_(const std::string &file_path); //IsRegularFile_, RequestError_, PathError_, AddCommonHeaders_, GetMimeType_
		std::string	GetMimeType_(const std::string &path) const; //PathExtension
		bool	IsRegularFile_(const std::string &path) const;
		std::string	PathExtension_(const std::string &path) const;


		// unique -> deps externas: base (AddCommonHeaders_, DefaultResponseHeader_)
		bool	IsValidPath_(const std::string &path) const;
		void	MovedPermanently_(const HttpRequest &request); // AddCommonHeaders_, DefaultResponseHeader_
		void	ListDirectory_(const std::string &request_path); // AddCommonHeaders_, TryAddDirectoryContent
		bool	TryAddDirectoryContent_(std::stringstream *body, // IsDirectory
										const std::string &full_path);
		bool	IsDirectory_(const std::string &path) const;

		RequestConfig *requestConfig_;
		HttpRequest *request_;
		bool	keep_alive_;
		std::size_t errCode;
		std::string raw_response_;
};

#endif  // SRCS_INCS_HTTPGETRESPONSE_HPP_
