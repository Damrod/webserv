#include "HttpRedirectionResponse.hpp"

HttpRedirectionResponse::HttpRedirectionResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request) {
	if (error_code_) {
		raw_response_ = HttpErrorResponse(
										error_code_,
										request_config_,
										request_).Content();
	} else {
		HttpResponse::HeadersMap headers;

		headers.insert(std::make_pair("Content-Type", "text/html"));
		headers.insert(std::make_pair("Location", request_config_->GetReturnUrl()));
		SetRawResponse_(request_config_->GetReturnStatus(), headers, "");
	}
}
