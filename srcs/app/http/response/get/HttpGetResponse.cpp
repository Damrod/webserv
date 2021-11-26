#include <HttpGetResponse.hpp>

HttpGetResponse::HttpGetResponse(
	RequestConfig *request_config,
	HttpRequest *request) :
	HttpBaseResponse(request_config, request) {
	if (error_code_) {
		SetErrorRawResponse_(error_code_);
	} else {
		const std::string full_path =
				request_config_->GetRoot() + request_->GetDecodedPath();
		try {
			File file(full_path);

			if (file.IsRegularFile()) {
				HandleRegularFile_(file);
			} else if (file.HasEndSlash()) {
				HandleSlashEndedFile_(file);
			} else {
				MovedPermanently_(*request_);
			}
		} catch(File::Error & e) {
			SetErrorRawResponse_(e.what());
		}
	}
}

void	HttpGetResponse::MovedPermanently_(const HttpRequest &request) {
	HttpResponse::HeadersMap headers;
	std::stringstream url;

	url	<< "http://"
		<< request.GetHost()
		<< ":"
		<< request.GetPort()
		<< request.GetDecodedPath()
		<< "/";

	headers.insert(std::make_pair("Content-Type", "text/html"));
	headers.insert(std::make_pair("Location", url.str()));
	SetRawResponse_(301, headers, "");
}

void	HttpGetResponse::ListDirectory_(
								File file,
								const std::string &request_path) {
	HttpResponse::HeadersMap headers;
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
	SetRawResponse_(200, headers, body);
}

void	HttpGetResponse::HandleRegularFile_(File file) {
	if (request_config_->HasCGI(file.GetPathExtension())) {
		try {
			ExecuteCGI_(file);
		} catch (const std::exception &e) {
			SetErrorRawResponse_(500);
		}
	} else {
		Serve_(file);
	}
}

void	HttpGetResponse::HandleSlashEndedFile_(File file) {
	if (request_config_->HasAutoindex()) {
		ListDirectory_(file, request_->GetDecodedPath());
	} else {
		file.SetSubpath(request_config_->GetIndex());
		if (file.IsRegularFile()) {
			HandleRegularFile_(file);
		} else {
			SetErrorRawResponse_(403);
		}
	}
}

void	HttpGetResponse::SetErrorRawResponse_(int error_code) {
	raw_response_ = HttpErrorResponse(
									error_code,
									request_config_,
									request_).Content();
}
