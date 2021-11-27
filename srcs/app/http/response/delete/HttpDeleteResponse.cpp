#include <HttpDeleteResponse.hpp>

HttpDeleteResponse::HttpDeleteResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : HttpBaseResponse(requestConfig, request) {

	if (error_code_) {
		SetErrorRawResponse_(error_code_);
	}
	const std::string full_path =
			request_config_->GetRoot() + request_->GetDecodedPath();
	try {
		File file(full_path);

		if (file.IsRegularFile()) {
			//HandleRegularFile_(file);
		} else if (file.HasEndSlash()) {
			//HandleSlashEndedFile_(file);
		} else {
			//MovedPermanently_(*request_);
		}
	} catch(File::Error & e) {
		SetErrorRawResponse_(e.what());
	}
	// else {
	// 	HttpResponse::HeadersMap headers;
	// 	std::string	body = "Responding to a DELETE request\n";

	// 	headers.insert(std::make_pair("Content-Type", "text/plain"));
	// 	headers.insert(std::make_pair("Location", request_config_->GetReturnUrl()));
	// 	SetRawResponse_(200, headers, body);
	// }
}

void	HttpDeleteResponse::SetErrorRawResponse_(int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
