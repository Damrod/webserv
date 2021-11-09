#include <HttpResponseFactory.hpp>

HttpResponseFactory::HttpResponseFactory(
		HttpRequest *request,
		RequestConfig *requestConfig
	): request_(request), requestConfig_(requestConfig) {
	concreteResponses_.insert(std::make_pair(
		"GET",
		&HttpResponseFactory::createHttpGetResponse_
	));
	concreteResponses_.insert(std::make_pair(
		"POST",
		&HttpResponseFactory::createHttpPostResponse_
	));
	concreteResponses_.insert(std::make_pair(
		"DELETE",
		&HttpResponseFactory::createHttpDeleteResponse_
	));
}

HttpResponseFactory::~HttpResponseFactory() {
}

IResponse *HttpResponseFactory::response() {
	if (!request_ || request_->GetState() == RequestState::kInvalid) {
		return createHttpErrorResponse_(400);
	}
	if (!requestConfig_->getReturnUrl().empty()) {
		return createHttpRedirectionResponse_();
	}

	std::map<std::string, responseCreatorMethod>::const_iterator it;
	it = concreteResponses_.find(request_->GetMethod());
	if (it == concreteResponses_.end()) {
		return createHttpErrorResponse_(501);
	} else {
		return CALL_MEMBER_FN(*this, it->second)();
	}
}

bool	HttpResponseFactory::hasAcceptedFormat_(HttpRequest *request_) {
	return true;
}

IResponse *HttpResponseFactory::createHttpRedirectionResponse_() {
	return new HttpRedirectionResponse(requestConfig_, request_);
	// delete requestConfig_;
	// requestConfig_ = NULL;
}

IResponse *HttpResponseFactory::createHttpGetResponse_() {
	return new HttpGetResponse(requestConfig_, request_);
	// delete requestConfig_;
	// requestConfig_ = NULL;
}

IResponse *HttpResponseFactory::createHttpPostResponse_() {
	return new HttpPostResponse(requestConfig_, request_);
	// delete requestConfig_;
	// requestConfig_ = NULL;
}

IResponse *HttpResponseFactory::createHttpDeleteResponse_() {
	return new HttpDeleteResponse(requestConfig_, request_);
	// delete requestConfig_;
	// requestConfig_ = NULL;
}

IResponse *HttpResponseFactory::createHttpErrorResponse_(int statusCode) {
	// Responsabilidad de la respuesta
	// SetKeepAlive_(*request);
	// hasAcceptedFormat_(request_)

	// Tiene que devolver un error
	// Pasar el código (ver cómo)

	// delete requestConfig_;
	// requestConfig_ = NULL;
}
