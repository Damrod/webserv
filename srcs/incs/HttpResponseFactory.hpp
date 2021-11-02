#ifndef SRCS_INCS_HTTPRESPONSEFACTORY_HPP_
#define SRCS_INCS_HTTPRESPONSEFACTORY_HPP_

#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))

#include "IResponseFactory.hpp"
#include "HttpRequestDTO.hpp"
#include "HttpGetResponse.hpp"
#include "HttpPostResponse.hpp"
#include "HttpDeleteResponse.hpp"
#include "HttpRedirectionResponse.hpp"

class HttpResponseFactory: public IResponseFactory {
	public:
		HttpResponseFactory(const std::string raw_request);
		~HttpResponseFactory();
		IResponse *response();

	private:
		HttpResponseFactory();

		typedef  IResponse *(HttpResponseFactory::* responseCreatorMethod)();
		std::map<std::string, responseCreatorMethod>		concreteResponses_;
		HttpRequestDTO										*request_;
		std::string											request_method_;
		IResponse											*createHttpGetResponse_();
		IResponse											*createHttpPostResponse_();
		IResponse											*createHttpDeleteResponse_();
		IResponse											*createHttpRedirectionResponse_();
};

#endif //SRCS_INCS_HTTPRESPONSEFACTORY_HPP_
