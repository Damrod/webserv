#include "HttpGetResponse.hpp"

HttpGetResponse::HttpGetResponse(
	RequestConfig *request_config,
	HttpRequest *request) :
	HttpBaseResponse(request_config, request)
{
	const std::string full_path =
							request_config_->GetRoot() + request_->GetPath();
	try {
		File file(full_path);

		// Encapsular lógica en alguna función privada
		// para que no quede tan feo
		if (file.IsRegularFile()) {
			if (request_config_->HasCGI(file.GetPathExtension())) {
				ExecuteCGI_(*request_, file);
			} else {
				if (file.IsRegularFile()) {
					Serve_(file);
				} else {
					raw_response_ = HttpErrorResponse(
													403,
													request_config_,
													request_
													).content();
				}
			}
		}
		else if (file.HasEndSlash()) {
			if (request_config_->HasAutoindex()) {
				ListDirectory_(file, request_->GetPath());
			} else {
				file.SetSubpath(request_config_->GetIndex());
				Serve_(file);
			}
		} else {
			MovedPermanently_(*request_);
		}
	} catch(File::Error & e) {
		raw_response_ = HttpErrorResponse(
										e.what(),
										request_config_,
										request_
										).content();
	}
}

std::string HttpGetResponse::content() {
	return raw_response_;
}

// Common
void	HttpGetResponse::ExecuteCGI_(const HttpRequest &request, File file) {
	try {
		CGI engine(request, *request_config_, file.GetPathExtension());
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
		raw_response_ = HttpErrorResponse(
										500,
										request_config_,
										request_
										).content();
	}
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
