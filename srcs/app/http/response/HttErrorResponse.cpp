#include "HttpErrorResponse.hpp"

HttpErrorResponse::HttpErrorResponse(
	int error_code,
	RequestConfig *requestConfig,
	HttpRequest *request) :
	HttpBaseResponse(requestConfig, request) {
	error_code_ = error_code;
	const std::string error_page_path =
		request_config_->GetErrorPagePath(error_code_);

	File file(error_page_path);
	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code_);
	} else {
		Serve_(file);
	}
}
