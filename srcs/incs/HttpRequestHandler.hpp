#ifndef SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#define SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequestHandler.hpp>
#include <MimeTypes.hpp>
#include <RequestLocation.hpp>
#include <ServerConfig.hpp>

class HttpRequestHandler : public IRequestHandler {
	public:
		HttpRequestHandler(const ServerConfig &server_config,
							const std::string &raw_request);
		~HttpRequestHandler();
		std::string			GetRawResponse() const;
		bool				GetKeepAlive() const;

	private:
		HttpRequestHandler();
		HttpRequestHandler(const HttpRequestHandler &);
		HttpRequestHandler&	operator=(const HttpRequestHandler&);

		const ServerConfig	&server_config_;
		std::string			raw_request_;
		std::string			raw_response_;
		bool				keep_alive_;
		RequestLocation		*request_location_;

		void				SetKeepAlive_(const HttpRequest &request);
		void				DoRedirection_();
		void				HandleRequest_();
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
