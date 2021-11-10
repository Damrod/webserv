#include "HttpPostResponse.hpp"

std::string HttpPostResponse::content() {
	//checkKeepAlive
	//HasAcceptedFormat
	const std::string request_path = request_->GetPath();
	const std::string full_path =
							requestConfig_->GetRoot() + request_path;
	if (IsRegularFile_(full_path)) {
		if (IsCGI_(full_path)) {
			ExecuteCGI_(*request_, full_path);
		} else {
			RequestError_(501);
		}
	} else {
		if (IsUploadEnabled_() && IsValidUploadPath_(request_path)) {
			UploadFile_(*request_);
		} else {
			RequestError_(404);
		}
	}
}

void		HttpPostResponse::CheckKeepAlive_() {
	if (request_->HasHeader("Connection") &&
			ToLowerString(request_->GetHeaderValue("Connection")) == "close") {
		keep_alive_ = false;
	}
}

bool	HttpPostResponse::HasAcceptedFormat_(const HttpRequest &request) {
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

bool	HttpPostResponse::IsRegularFile_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFREG) {
		return true;
	}
	return false;
}

std::string	HttpPostResponse::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

void	HttpPostResponse::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate_());
}

std::string	HttpPostResponse::PathExtension_(const std::string &path) const {
	const std::size_t extension_position = path.rfind(".");
	if (extension_position == std::string::npos || extension_position < 2) {
		return "";
	}
	const std::size_t	last_dir_position = path.rfind("/");
	if (last_dir_position == std::string::npos) {
		return path.substr(extension_position + 1);
	}
	if (last_dir_position < extension_position - 1) {
		const std::string last_path_part = path.substr(last_dir_position + 1);
		if (last_path_part != "." && last_path_part != "..") {
			return path.substr(extension_position + 1);
		}
	}
	return "";
}

void	HttpPostResponse::ExecuteCGI_(const HttpRequest &request,
					const std::string &full_path) {
	try {
		HttpResponse response(200);
		AddCommonHeaders_(&response);
		CGI engine(request, *requestConfig_, PathExtension_(full_path),
			&response);
		engine.ExecuteCGI();
		if (engine.GetExecReturn() != EXIT_SUCCESS) {
			throw std::runtime_error("Exec error");
	}
	raw_response_ = response.CreateResponseString();
	} catch (const std::exception &e) {
		RequestError_(500);
	}
}

bool	HttpPostResponse::IsCGI_(const std::string &full_path) const {
	return requestConfig_->HasCGI(PathExtension_(full_path)) > 0;
}

std::string	HttpPostResponse::DefaultResponseBody_(
								const std::size_t status_code) const {
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

void	HttpPostResponse::DefaultStatusResponse_(
									const std::size_t status_code) {
	HttpResponse		response(status_code);
	response.SetBody(DefaultResponseBody_(status_code));
	response.AddHeader("Content-Type", "text/html");
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpPostResponse::PathError_() {
	if (errno == ENOENT || errno == ENOTDIR) {
		RequestError_(404);
	} else if (errno == EACCES) {
		RequestError_(403);
	} else {
		RequestError_(500);
	}
}

std::string	HttpPostResponse::GetMimeType_(const std::string &path) const {
	return MimeTypes::GetMimeType(PathExtension_(path));
}

void	HttpPostResponse::ServeFile_(const std::string &file_path) {
	if (!IsRegularFile_(file_path)) {
		RequestError_(403);
		return;
	}
	std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) {
		PathError_();
		return;
	}
	const std::string body = std::string(std::istreambuf_iterator<char>(ifs),
											std::istreambuf_iterator<char>());
	HttpResponse response(200);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", GetMimeType_(file_path));
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

void	HttpPostResponse::RequestError_(const std::size_t error_code) {
	const std::string error_page_path =
		requestConfig_->GetErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		ServeFile_(error_page_path);
	}
}

bool	HttpPostResponse::IsUploadEnabled_() const {
	return !requestConfig_->GetUploadStore().empty();
}

bool	HttpPostResponse::IsValidUploadPath_(const std::string &path) const {
	return path == requestConfig_->GetPath();
}

void	HttpPostResponse::UploadFile_(const HttpRequest &request) {
	try {
		FormFile form_file(request);
		const std::string full_upload_path =
								requestConfig_->GetUploadStore() +
								form_file.GetFilename();
		std::ofstream out(full_upload_path.c_str());
		if (!out) {
			PathError_();
			return;
		}
		const std::string file_content = form_file.GetFileContent();
		out.write(file_content.c_str(), file_content.size());
		if (!out) {
			RequestError_(500);
		} else {
			DefaultStatusResponse_(200);
		}
	}
	catch (const std::exception &e) {
		RequestError_(400);
	}
}
