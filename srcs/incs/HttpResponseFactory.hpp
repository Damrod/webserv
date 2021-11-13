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
	public:
		HttpResponseFactory(
			HttpRequest *request,
			const ServerConfig &server_config);
		~HttpResponseFactory();
		IResponse *Response();

	private:
		HttpResponseFactory();
		bool	isKeepAlive_();
		bool	hasAcceptedFormat_(HttpRequest *request_);
		IResponse	*createHttpGetResponse_();
		IResponse	*createHttpPostResponse_();
		IResponse	*createHttpDeleteResponse_();
		IResponse	*createHttpRedirectionResponse_();
		IResponse	*createHttpErrorResponse_(int statusCode);
		void	SetRequestConfig_();

		typedef  IResponse *(HttpResponseFactory::* responseCreatorMethod)();
		std::map<std::string, responseCreatorMethod>	concrete_responses_;
		const ServerConfig &server_config_;
		HttpRequest	*request_;
		RequestConfig	*request_config_;
};

#endif  // SRCS_INCS_HTTPRESPONSEFACTORY_HPP_

// 1. Extrapolar la lógica de creación a Factory HTTP-respnse
// 2. Extraer métodos comunes a una clase común HttpRequest
// Tiene que manejar config también.
