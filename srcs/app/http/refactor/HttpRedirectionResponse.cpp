#include "HttpRedirectionResponse.hpp"

HttpRedirectionResponse::HttpRedirectionResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request)
{
	AHttpResponse::HeadersMap headers;

	headers.insert(std::make_pair("Content-Type", "text/html"));
	headers.insert(std::make_pair("Location", request_config_->GetReturnUrl()));
	SetRawResponse_(request_config_->GetReturnStatus(), headers, NULL);
}

std::string HttpRedirectionResponse::content() {
	return raw_response_;
}
