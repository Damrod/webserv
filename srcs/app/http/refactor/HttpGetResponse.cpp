#include "HttpGetResponse.hpp"

HttpGetResponse::HttpGetResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) :
	requestConfig_(requestConfig),
	request_(request)
{
	CheckKeepAlive_();
	//HasAcceptedFormat
	const std::string full_path =
							requestConfig_->GetRoot() + request_->GetPath();
	try {
		File file(full_path);

		// Encapsular lógica en alguna función privada
		// para que no quede tan feo
		if (file.IsRegularFile()) {
			requestConfig_->HasCGI(file.GetPathExtension())
				? ExecuteCGI_(*request_, file)
				: Serve_(file);
		}
		else if (file.HasEndSlash()) {
			if (requestConfig_->HasAutoindex()) {
				ListDirectory_(file, request_->GetPath());
			} else {
				file.SetSubpath(requestConfig_->GetIndex());
				Serve_(file);
			}
		} else {
			MovedPermanently_(*request_);
		}
	} catch(File::Error & e) {
		RequestError_(e.what());
	}
}

std::string HttpGetResponse::content() {
	return raw_response_;
}

void		HttpGetResponse::CheckKeepAlive_() {
	if (request_->HasHeader("Connection") &&
			ToLowerString(request_->GetHeaderValue("Connection")) == "close") {
		keep_alive_ = false;
	}
}

bool	HttpGetResponse::HasAcceptedFormat_(const HttpRequest &request) {
	if (requestConfig_->Limits(request.GetMethod())) {
			errCode = 405;
	}
	if (request.GetBody().size() >
							requestConfig_->GetClientMaxBodySize()) {
		errCode = 413;
	}
	if (request.HasHeader("Content-Encoding")) {
		errCode = 415;
	}
}

// errores
void	HttpGetResponse::RequestError_(const std::size_t error_code) {
	const std::string error_page_path =
		requestConfig_->GetErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		Serve_(error_page_path);
	}
}

// métodos generadores de respuesta

void
HttpGetResponse::DefaultStatusResponse_(const std::size_t status_code) {
	AHttpResponse::HeadersMap headers;
	headers.insert(std::make_pair("Content-Type", "text/html"));

	raw_response_ = AHttpResponse(
								status_code,
								headers,
								NULL,
								keep_alive_).RawContent();
}

// Common
void	HttpGetResponse::ExecuteCGI_(const HttpRequest &request, File file) {
	try {
		CGI engine(request, *requestConfig_, file.GetPathExtension());
		engine.ExecuteCGI();
		if (engine.GetExecReturn() != EXIT_SUCCESS) {
			throw std::runtime_error("Exec error");
		}
		raw_response_ = AHttpResponse(
									200,
									engine.GetHeaders(),
									engine.GetBody(),
									keep_alive_).RawContent();
	} catch (const std::exception &e) {
		RequestError_(500);
	}
}

void	HttpGetResponse::Serve_(File file) {
	AHttpResponse::HeadersMap headers;
	std::string body;

	if (!file.IsRegularFile()) {
		RequestError_(403);
		return;
	}
	// headers.insert(std::make_pair("Content-Type", "text/html"));
	headers.insert(std::make_pair("Content-Type", file.GetMimeType()));
	body = file.GetContent();
	raw_response_ = AHttpResponse(
								200,
								headers,
								body,
								keep_alive_).RawContent();
}


// GET specific
void	HttpGetResponse::MovedPermanently_(const HttpRequest &request) {
	AHttpResponse::HeadersMap headers;
	std::stringstream url;

	url	<< "http://"
		<< request.GetHost()
		<< ":"
		<< request.GetPort()
		<< request.GetPath()
		<< "/";

	headers.insert(std::make_pair("Content-Type", "text/html"));
	headers.insert(std::make_pair("Location", url.str()));
	raw_response_ = AHttpResponse(
								301,
								headers,
								NULL,
								keep_alive_).RawContent();
}

void	HttpGetResponse::ListDirectory_(File file, const std::string &request_path) {
	AHttpResponse::HeadersMap headers;
	std::stringstream bodyStream;
	std::string body;
	std::string full_path;

	headers.insert(std::make_pair("Content-Type", "text/html"));
	bodyStream << "<html>\n"
		<< "<head><title>Index of "
		<< request_path
		<< "</title></head>\n"
		<< "<body>\n"
		<< "<h1>Index of "
		<< request_path
		<< "</h1><hr><pre><a href=\"../\">../</a>\n"
		<< file.GetDirectoryContent()
		<< "</pre><hr></body>\n"
		<< "</html>\n";
	body = bodyStream.str();
	raw_response_ = AHttpResponse(
								200,
								headers,
								body,
								keep_alive_).RawContent();
}
