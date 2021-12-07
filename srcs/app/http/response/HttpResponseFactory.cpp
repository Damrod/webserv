#include <HttpResponseFactory.hpp>

HttpResponseFactory::HttpResponseFactory(
		HttpRequest *request,
		HttpResponseFactory::serverSettingsMap *server_config):
		request_(request),
		server_config_(server_config),
		request_config_(NULL) {
	concrete_responses_.insert(std::make_pair(
		"GET",
		&HttpResponseFactory::createHttpGetResponse_
	));
	concrete_responses_.insert(std::make_pair(
		"POST",
		&HttpResponseFactory::createHttpPostResponse_
	));
	concrete_responses_.insert(std::make_pair(
		"DELETE",
		&HttpResponseFactory::createHttpDeleteResponse_
	));
}

HttpResponseFactory::~HttpResponseFactory() {
	delete request_config_;
}

IResponse *HttpResponseFactory::Response() {
	SetRequestConfig_();
	if (!request_ || !request_->IsComplete()) {
		return createHttpErrorResponse_(400);
	}
	if (!request_config_->GetReturnUrl().empty()) {
		return createHttpRedirectionResponse_();
	}

	std::map<std::string, responseCreatorMethod>::const_iterator it;
	it = concrete_responses_.find(request_->GetMethod());
	if (it == concrete_responses_.end()) {
		return createHttpErrorResponse_(501);
	} else {
		return CALL_MEMBER_FN(*this, it->second)();
	}
}

void	HttpResponseFactory::SetRequestConfig_() {
	delete request_config_;
	const ServerConfig &serverName_config = GetServerNameConfig_();
	request_config_ =
				new RequestConfig(serverName_config, request_->GetDecodedPath());
}

ServerConfig &HttpResponseFactory::GetServerNameConfig_(void) {
	std::string host_name = request_->GetHost();
	HttpResponseFactory::serverSettingsMap::iterator it;

	it = server_config_->find(host_name);
	// Comprobar wildcard al chequear severNames petición
	if (it != server_config_->end()) {
		return *it->second;
	} else {
		return *server_config_->begin()->second;
	}
}



IResponse *HttpResponseFactory::createHttpRedirectionResponse_() {
	return new HttpRedirectionResponse(request_config_, request_);
}

IResponse *HttpResponseFactory::createHttpGetResponse_() {
	return new HttpGetResponse(request_config_, request_);
}

IResponse *HttpResponseFactory::createHttpPostResponse_() {
	return new HttpPostResponse(request_config_, request_);
}

IResponse *HttpResponseFactory::createHttpDeleteResponse_() {
	return new HttpDeleteResponse(request_config_, request_);
}

IResponse *HttpResponseFactory::createHttpErrorResponse_(int statusCode) {
	return new HttpErrorResponse(statusCode, request_config_, request_);
}
