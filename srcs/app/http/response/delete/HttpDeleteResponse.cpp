#include <HttpDeleteResponse.hpp>

HttpDeleteResponse::HttpDeleteResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request) {
	// TODO(any) Implement DELETE
	if (error_code_) {
		raw_response_ = HttpErrorResponse(
										error_code_,
										request_config_,
										request_).Content();
	} else {
		HttpResponse::HeadersMap headers;
		std::string	body = "Responding to a DELETE request\n";

		headers.insert(std::make_pair("Content-Type", "text/plain"));
		headers.insert(std::make_pair("Location", request_config_->GetReturnUrl()));
		SetRawResponse_(request_config_->GetReturnStatus(), headers, body);
	}
}
