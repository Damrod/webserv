#ifndef SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#define SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#include <HttpRequestHandler.hpp>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <iostream>
#include <ctime>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <FormFile.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <HttpStatusCodes.hpp>
#include <IRequest.hpp>
#include <IRequestHandler.hpp>
#include <MimeTypes.hpp>
#include <RequestLocation.hpp>
#include <ServerConfig.hpp>
#include <StringUtils.hpp>
#include <CGI.hpp>


class HttpRequestHandler : public IRequestHandler {
	public:
		explicit	HttpRequestHandler(const ServerConfig &server_config);
		~HttpRequestHandler();
		std::string			BuildResponse(IRequest *request);
		bool				GetKeepAlive() const;

	private:
		HttpRequestHandler();
		HttpRequestHandler(const HttpRequestHandler &);
		HttpRequestHandler&	operator=(const HttpRequestHandler&);

		const ServerConfig	&server_config_;
		std::string			raw_response_;
		bool				keep_alive_;
		RequestLocation		*request_location_;

		void				SetKeepAlive_(const HttpRequest &request);
		void				DoRedirection_();
		void				HandleRequest_(const HttpRequest *request);
		void				HandleMethod_(const HttpRequest &request);
		void				AddCommonHeaders_(HttpResponse *response);
		std::string			DefaultResponseBody_(
										const std::size_t status_code) const;
		void				DefaultStatusResponse_(
												const std::size_t status_code);
		void				RequestError_(const std::size_t error_code);
		void				PathError_();
		bool				TryAddDirectoryContent_(std::stringstream *ss,
												const std::string &full_path);
		std::string			GetFullPath_(const std::string &request_path) const;
		void				ListDirectory_(const std::string &request_path);
		bool				HasAcceptedFormat_(const HttpRequest &request);
		void				ServeFile_(const std::string &file_path);
		void				MovedPermanently_(const HttpRequest &request);
		void				DoGet_(const HttpRequest &request);
		bool				IsCGI_(const std::string &full_path) const;
		bool				IsUploadEnabled_() const;
		bool				IsValidUploadPath_(const std::string &path) const;
		void				UploadFile_(const HttpRequest &request);
		void				ExecuteCGI_(const HttpRequest &request,
										const std::string &full_path);
		void				DoPost_(const HttpRequest &request);
		void				DoDelete_(const HttpRequest &request);

		bool				IsValidPath_(const std::string &path) const;
		bool				IsDirectory_(const std::string &path) const;
		bool				IsRegularFile_(const std::string &path) const;
		std::string			PathExtension_(const std::string &path) const;
		std::string			GetMimeType_(const std::string &file_path) const;
		std::string			CurrentDate_() const;
};

#endif  // SRCS_INCS_HTTPREQUESTHANDLER_HPP_
