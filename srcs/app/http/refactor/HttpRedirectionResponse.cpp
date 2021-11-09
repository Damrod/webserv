#include "HttpRedirectionResponse.hpp"

HttpRedirectionResponse::HttpRedirectionResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : requestConfig_(requestConfig), request_(request) {}

std::string HttpRedirectionResponse::content() {
	HttpResponse response(requestConfig_->getReturnStatus());
	CheckKeepAlive_();
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	response.AddHeader("Location", requestConfig_->getReturnUrl());
	const std::string body =
		DefaultResponseBody_(requestConfig_->getReturnStatus());
	response.SetBody(body);
	return response.CreateResponseString();
}

// ---- utils ----

void		HttpRedirectionResponse::CheckKeepAlive_() {
	if (request_->HasHeader("Connection") &&
			ToLowerString(request_->GetHeaderValue("Connection")) == "close") {
		keep_alive_ = false;
	}
}

std::string	HttpRedirectionResponse::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

void	HttpRedirectionResponse::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate_());
}

std::string
HttpRedirectionResponse::DefaultResponseBody_(const std::size_t status_code) const {
	std::stringstream	ss;
	ss << status_code << " " << HttpStatusCodes::GetReasonPhrase(status_code);
	const std::string	message = ss.str();
	const std::string	body = "<html>\n"
						"<head><title>" + message + "</title></head>\n"
						"<body>\n"
						"<center><h1>" + message + "</h1></center>\n"
						"<hr><center>webserv</center>\n"
						"</body>\n"
						"</html>\n";
	return body;
}
