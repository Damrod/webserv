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
		DefaultStatusResponse_();
	} else {
		Serve_(error_page_path);
	}
}

std::string HttpErrorResponse::content() {
	return raw_response_;
}

void	HttpErrorResponse::DefaultStatusResponse_() {
	AHttpResponse::HeadersMap headers;
	headers.insert(std::make_pair("Content-Type", "text/html"));

	raw_response_ = AHttpResponse(
								error_code_,
								headers,
								NULL,
								keep_alive_).RawContent();
}
