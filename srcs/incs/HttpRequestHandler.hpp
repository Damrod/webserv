#ifndef SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#define SRCS_INCS_HTTPREQUESTHANDLER_HPP_
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequestHandler.hpp>
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
		void				DefaultErrorPage_(const std::size_t error_code);
		std::string			CurrentDate_() const;
		void				AddCommonHeaders_(HttpResponse *response);
		void				SetKeepAlive_(const HttpRequest &request);
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
		void				PathError_(const Location *location);
};

#endif  // SRCS_INCS_HTTPREQUESTHANDLER_HPP_
