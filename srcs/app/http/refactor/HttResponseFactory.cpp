#include "HttpResponseFactory.hpp"

HttpResponseFactory::HttpResponseFactory(const std::string raw_request) {
	request_ = new HttpRequestDTO(raw_request);
	request_method_ = request_->GetMethod();
	concreteResponses_.insert(std::make_pair("GET", &HttpResponseFactory::createHttpGetResponse_));
	concreteResponses_.insert(std::make_pair("POST", &HttpResponseFactory::createHttpPostResponse_));
	concreteResponses_.insert(std::make_pair("DELETE", &HttpResponseFactory::createHttpDeleteResponse_));
	concreteResponses_.insert(std::make_pair("REDIRECT", &HttpResponseFactory::createHttpRedirectionResponse_));
}

HttpResponseFactory::~HttpResponseFactory() {
}

IResponse *HttpResponseFactory::response() {
	std::map<std::string, responseCreatorMethod>::const_iterator it;

	it = concreteResponses_.find(request_method_);

	if (it != concreteResponses_.end()) {
		return CALL_MEMBER_FN(*this, it->second)();
	}
	//return "application/octet-stream";
	//Invalid method -> invalid response
}

IResponse *HttpResponseFactory::createHttpGetResponse_() {
	return new HttpGetResponse();
}

IResponse *HttpResponseFactory::createHttpPostResponse_() {
	return new HttpPostResponse();
}

IResponse *HttpResponseFactory::createHttpDeleteResponse_() {
	return new HttpDeleteResponse();
}

IResponse *HttpResponseFactory::createHttpRedirectionResponse_() {
	return new HttpRedirectionResponse();
}
