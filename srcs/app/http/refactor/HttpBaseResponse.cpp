#include <HttpBaseResponse.hpp>

HttpBaseResponse::HttpBaseResponse(
	RequestConfig *request_config,
	HttpRequest *request):
	request_config_(request_config),
	request_(request)
{
	keep_alive_ = (request_->HasHeader("Connection") &&
					ToLowerString(request_->GetHeaderValue("Connection")) == "close")
					? false
					: true;

	if (request_config_->Limits(request_->GetMethod())) {
		error_code_ = 405;
	} else if (request_->GetBody().size() >
							request_config_->GetClientMaxBodySize()) {
		error_code_ = 413;
	} else if (request_->HasHeader("Content-Encoding")) {
		error_code_ = 415;
	} else {
		error_code_ = 0;
	}
}

void	HttpBaseResponse::Serve_(File file) {
	AHttpResponse::HeadersMap headers;
	std::string body;

	headers.insert(std::make_pair("Content-Type", file.GetMimeType()));
	body = file.GetContent();
	SetRawResponse_(200, headers, body, keep_alive_);
}

void	HttpBaseResponse::ExecuteCGI_(File file) {
	CGI engine(*request_, *request_config_, file.GetPathExtension());
	engine.ExecuteCGI();
	if (engine.GetExecReturn() != EXIT_SUCCESS) {
		throw std::runtime_error("Exec error");
	}
	SetRawResponse_(200, engine.GetHeaders(), engine.GetBody(), keep_alive_);
}

void	HttpBaseResponse::SetRawResponse_(
										int code,
										AHttpResponse::HeadersMap headers,
										std::string body,
										bool keep_alive)
{
	raw_response_ = AHttpResponse(
								code,
								headers,
								body,
								keep_alive_).RawContent();
}
