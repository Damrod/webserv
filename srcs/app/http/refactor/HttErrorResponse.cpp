#include "HttpErrorResponse.hpp"

HttpErrorResponse::HttpErrorResponse(
	int error_code,
	RequestConfig *requestConfig,
	HttpRequest *request) :
	HttpBaseResponse(requestConfig, request),
	error_code_(error_code)
{
	const std::string error_page_path =
		requestConfig_->GetErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code_);
	} else {
		Serve_(error_page_path);
	}
}

std::string HttpErrorResponse::content() {
	return raw_response_;
}
