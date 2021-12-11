#ifndef SRCS_INCS_HTTPRESPONSEFACTORY_HPP_
#define SRCS_INCS_HTTPRESPONSEFACTORY_HPP_

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

#include <map>
#include <string>
#include <IResponseFactory.hpp>
#include <HttpRequest.hpp>
#include <HttpGetResponse.hpp>
#include <HttpPostResponse.hpp>
#include <HttpDeleteResponse.hpp>
#include <HttpErrorResponse.hpp>
#include <HttpRedirectionResponse.hpp>
#include <ServerConfig.hpp>
#include <RequestConfig.hpp>

class HttpResponseFactory: public IResponseFactory {
	private:
		typedef  IResponse *(HttpResponseFactory::* responseCreatorMethod)();
		typedef std::map<const std::string, ServerConfig *> serverSettingsMap;

	public:
		HttpResponseFactory(
			HttpRequest *request,
			serverSettingsMap *server_config);
		~HttpResponseFactory();
		IResponse *Response();

	private:
		HttpResponseFactory();
		IResponse	*createHttpGetResponse_();
		IResponse	*createHttpPostResponse_();
		IResponse	*createHttpDeleteResponse_();
		IResponse	*createHttpRedirectionResponse_();
		IResponse	*createHttpErrorResponse_(int statusCode);
		void		SetRequestConfig_();
		ServerConfig &GetServerNameConfig_(void);
 		serverSettingsMap::iterator FindWildcardServerName_(
			const std::string &host_name);

		std::map<std::string, responseCreatorMethod>	concrete_responses_;
		HttpRequest	*request_;
		serverSettingsMap *server_config_;
		RequestConfig	*request_config_;
};

#endif  // SRCS_INCS_HTTPRESPONSEFACTORY_HPP_
