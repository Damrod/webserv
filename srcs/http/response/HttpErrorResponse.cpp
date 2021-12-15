#include <HttpErrorResponse.hpp>

HttpErrorResponse::HttpErrorResponse(
	int error_code,
	RequestConfig *requestConfig,
	HttpRequest *request) :
	HttpBaseResponse(requestConfig, request) {
	error_code_ = error_code;
	const std::string error_page_path =
		request_config_->GetErrorPagePath(error_code_);

		if (error_page_path.empty()) {
			DefaultStatusResponse_(error_code_);
		} else {
			try {
				File file(error_page_path);

				Serve_(file, error_code_);
			} catch (File::Error &e ) {
				DefaultStatusResponse_(error_code_);
			}
		}
}
