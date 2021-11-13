#include <HttpBaseResponse.hpp>

HttpBaseResponse::HttpBaseResponse(
	RequestConfig *request_config,
	HttpRequest *request):
	request_config_(request_config),
	request_(request) {
    SetKeepAlive_();
	CheckReqFormat_();
}

void	HttpBaseResponse::SetKeepAlive_() {
	keep_alive_ = (request_->HasHeader("Connection") &&
	ToLowerString(request_->GetHeaderValue("Connection")) == "close")
	? false
	: true;
}

void HttpBaseResponse::CheckReqFormat_() {
	if (request_config_->Limits(request_->GetMethod())) {
			error_code_ = 405;
	} else if (request_->GetBody().size() >
							request_config_->GetClientMaxBodySize()) {
		error_code_ = 413;
	} else if (request_->HasHeader("Content-Encoding")) {
		error_code_ = 415;
	}
}

void	HttpBaseResponse::Serve_(File file) {
	AHttpResponse::HeadersMap headers;
	std::string body;

	headers.insert(std::make_pair("Content-Type", file.GetMimeType()));
	body = file.GetContent();
	raw_response_ = AHttpResponse(
								200,
								headers,
								body,
								keep_alive_).RawContent();
}
