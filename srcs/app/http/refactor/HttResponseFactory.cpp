#include "HttpResponseFactory.hpp"

HttpResponseFactory::HttpResponseFactory(
		const std::string raw_request,
		const ServerConfig &server_config
	): server_config_(server_config) {
	request_ = new HttpRequestDTO(raw_request);
	request_method_ = request_->GetMethod();
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
	if (request_ == NULL || request_->GetState() != RequestState::kComplete) {
		return createHttpErrorResponse_(400);
	}
	if (!server_config_.common.return_url.empty()) {
		return createHttpRedirectionResponse_();
	}

	request_location_ = new RequestLocation(server_config_, request_->GetPath());
	if (!request_location_->common.return_url.empty()) {
		return createHttpRedirectionResponse_();
	} else if (HasAcceptedFormat_()) {
		std::map<std::string, responseCreatorMethod>::const_iterator it;
		it = concreteResponses_.find(request_method_);
		if (it != concreteResponses_.end()) {
			return CALL_MEMBER_FN(*this, it->second)();
		} else {
			return createHttpErrorResponse_(501);
		}
	}
}

bool	HttpResponseFactory::HasAcceptedFormat_() {
	// if (request_location_->HasLocation() &&
	// 								!request_location_->limit_except->empty()) {
	// 	if (std::find(request_location_->limit_except->begin(),
	// 			request_location_->limit_except->end(),
	// 			request_->GetMethod()) ==
	// 								request_location_->limit_except->end()) {
	// 		RequestError_(405);
	// 		return false;
	// 	}
	// }
	// if (request_->GetBody().size() >
	// 						request_location_->common.client_max_body_size) {
	// 	RequestError_(413);
	// 	return false;
	// }
	// if (request_->HasHeader("Content-Encoding")) {
	// 	RequestError_(415);
	// 	return false;
	// }
	return true;
}

// la idea es pasar esto al objeto respuesta para que lo gestione
bool		HttpResponseFactory::isKeepAlive_() {
	return (request_->HasHeader("Connection") &&
			ToLowerString(request_->GetHeaderValue("Connection")) == "keep-alive");
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

IResponse *HttpResponseFactory::createHttpErrorResponse_(int statusCode) {
	// Tiene que devolver un error
	// Pasar el código (ver cómo)
}

