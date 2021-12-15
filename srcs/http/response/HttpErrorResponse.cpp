#include <HttpErrorResponse.hpp>

HttpErrorResponse::HttpErrorResponse(
	int error_code,
	RequestConfig *requestConfig,
	HttpRequest *request) :
	HttpBaseResponse(requestConfig, request) {
	status_code_ = error_code;
	const std::string error_page_path =
		request_config_->GetErrorPagePath(status_code_);

		if (error_page_path.empty()) {
			DefaultStatusResponse_(status_code_);
		} else {
			try {
				File file(error_page_path);

				Serve_(file);
			} catch (File::Error &e ) {
				DefaultStatusResponse_(status_code_);
			}
		}
}
