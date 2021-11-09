#include "HttpDeleteResponse.hpp"

HttpDeleteResponse::HttpDeleteResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : requestConfig_(requestConfig), request_(request) {}

std::string	HttpDeleteResponse::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

void	HttpDeleteResponse::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate_());
}

std::string HttpDeleteResponse::content() {
	// TODO(any) Implement DELETE
	// SetKeepAlive_(*request);
	// hasAcceptedFormat_(request_)
	(void)request_;
	HttpResponse	response(200);
	std::string		body = "Responding to a DELETE request\n";

	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(body);
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}
