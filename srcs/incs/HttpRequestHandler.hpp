#ifndef SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#define SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequestHandler.hpp>
#include <MimeTypes.hpp>
#include <ServerConfig.hpp>

class HttpRequestHandler : public IRequestHandler {
	public:
		HttpRequestHandler(const ServerConfig &server_config,
							const std::string &raw_request);
		std::string			GetRawResponse() const;
		bool				GetKeepAlive() const;

	private:
		const ServerConfig	&server_config_;
		std::string			raw_request_;
		std::string			raw_response_;
		bool				keep_alive_;

		void				HandleRequest_();
		const Location		*FindLocation_(
										const std::string &request_path) const;
		void				RequestError_(const Location *location,
												const std::size_t error_code);
		std::string			DefaultResponseBody_(
											const std::string &message) const;
		void				DefaultStatusResponse_(
												const std::size_t status_code);
		std::string			CurrentDate_() const;
		void				AddCommonHeaders_(HttpResponse *response);
		void				SetKeepAlive_(const HttpRequest &request);
		const CommonConfig	&GetCommonConfig(const Location *location) const;
		bool				HasAcceptedFormat_(const Location *location,
												const HttpRequest &request);
		std::string			PathExtension_(const std::string &path) const;
		void				DoGet_(const Location *location,
									const HttpRequest &request);
		void				DoPost_(const Location *location,
									const HttpRequest &request);
		void				DoDelete_(const Location *location,
										const HttpRequest &request);
		std::string			GetFullPath_(const Location *location,
										const std::string &request_path) const;
		void				ListDirectory_(const Location *location,
											const std::string &request_path);
		void				ServeFile_(const Location *location,
												const std::string &file_path);
		void				MovedPermanently_(const HttpRequest &request,
												const std::string &index_path);
		void				AddDirectoryContent_(std::stringstream *ss,
											const Location *location,
											const std::string &full_path);
		void				PathError_(const Location *location);
		bool				IsValidPath_(const std::string &path) const;
		bool				IsDirectory_(const std::string &path) const;
		bool				IsRegularFile_(const std::string &path) const;
		std::string			GetMimeType_(const std::string &file_path) const;

		static const MimeTypes	kMimeTypes_;
};

#endif  // SRCS_INCS_HTTPREQUESTHANDLER_HPP_
