#include "HttpGetResponse.hpp"

HttpGetResponse::HttpGetResponse(
	RequestConfig *requestConfig,
	HttpRequest *request) : requestConfig_(requestConfig), request_(request) {
	//checkKeepAlive
	//HasAcceptedFormat
	const std::string full_path =
							requestConfig_->GetRoot() + request_->GetPath();
	if (!IsValidPath_(full_path)) {
		// Gestión de errores
		PathError_();
		return;
	}
	if (IsRegularFile_(full_path)) {
		if (IsCGI_(full_path)) {
			ExecuteCGI_(*request_, full_path);
		} else {
			ServeFile_(full_path);
		}
	} else {
		const bool has_end_slash = full_path[full_path.size() - 1] == '/';
		if (!has_end_slash) {
			MovedPermanently_(*request_);
			return;
		}
		const std::string index_path =
									full_path + requestConfig_->GetIndex();
		if (requestConfig_->HasAutoindex() && !IsRegularFile_(index_path)) {
			ListDirectory_(request_->GetPath());
		} else {
			ServeFile_(index_path);
		}
	}
}

std::string HttpGetResponse::content() {
	// return response.CreateResponseString();
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

bool	HttpGetResponse::IsValidPath_(const std::string &path) const {
	struct stat statbuf;
	return stat(path.c_str(), &statbuf) == 0;
}

void	HttpGetResponse::PathError_() {
	if (errno == ENOENT || errno == ENOTDIR) {
		RequestError_(404);
	} else if (errno == EACCES) {
		RequestError_(403);
	} else {
		RequestError_(500);
	}
}

bool	HttpGetResponse::IsRegularFile_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFREG) {
		return true;
	}
	return false;
}

bool	HttpGetResponse::IsCGI_(const std::string &full_path) const {
	return requestConfig_->HasCGI(PathExtension_(full_path)) > 0;
}

std::string	HttpGetResponse::GetMimeType_(const std::string &path) const {
	return MimeTypes::GetMimeType(PathExtension_(path));
}

std::string	HttpGetResponse::DefaultResponseBody_(
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

void	HttpGetResponse::MovedPermanently_(const HttpRequest &request) {
	HttpResponse response(301);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	std::stringstream url;
	url << "http://" << request.GetHost() << ":" << request.GetPort() <<
		request.GetPath() << "/";
	response.AddHeader("Location", url.str());
	const std::string body = DefaultResponseBody_(301);
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

bool	HttpGetResponse::IsDirectory_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFDIR) {
		return true;
	}
	return false;
}

bool	HttpGetResponse::TryAddDirectoryContent_(std::stringstream *body,
											const std::string &full_path) {
	DIR *dir = opendir(full_path.c_str());
	if (dir == NULL) {
		PathError_();
		return false;
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name.rfind(".", 0) == 0) {
			continue;
		}
		const std::string full_path_name = full_path + "/" + name;
		if (IsDirectory_(full_path_name)) {
				name.append("/");
		}
		*body << "<a href=\"" << name << "\">" << name << "</a>\n";
	}
	closedir(dir);
	return true;
}

std::string	HttpGetResponse::PathExtension_(const std::string &path) const {
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

void	HttpGetResponse::RequestError_(const std::size_t error_code) {
	const std::string error_page_path =
		requestConfig_->GetErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		ServeFile_(error_page_path);
	}
}

// Migrado
std::string	HttpGetResponse::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

// Migrado
void	HttpGetResponse::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate_());
}

void	HttpGetResponse::RequestError_(const std::size_t error_code) {
	const std::string error_page_path =
		requestConfig_->GetErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		ServeFile_(error_page_path);
	}
}

// métodos generadores de respuesta

void
HttpGetResponse::DefaultStatusResponse_(const std::size_t status_code) {
	HttpResponse		response(status_code);
	response.SetBody(DefaultResponseBody_(status_code));
	response.AddHeader("Content-Type", "text/html");
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}


// Common
void	HttpGetResponse::ExecuteCGI_(const HttpRequest &request,
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

// Common
void	HttpGetResponse::ServeFile_(const std::string &file_path) {
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

// GET specific
void	HttpGetResponse::MovedPermanently_(const HttpRequest &request) {
	HttpResponse response(301);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	std::stringstream url;
	url << "http://" << request.GetHost() << ":" << request.GetPort() <<
		request.GetPath() << "/";
	response.AddHeader("Location", url.str());
	const std::string body = DefaultResponseBody_(301);
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

// GET specific
void	HttpGetResponse::ListDirectory_(const std::string &request_path) {
	std::stringstream body;
	body << "<html>\n" <<
		"<head><title>Index of " << request_path << "</title></head>\n" <<
		"<body>\n" <<
		"<h1>Index of " << request_path <<
		"</h1><hr><pre><a href=\"../\">../</a>\n";
	const std::string full_path = requestConfig_->GetRoot() + request_path;
	if (!TryAddDirectoryContent_(&body, full_path)) {
		return;
	}
	body << "</pre><hr></body>\n" <<
		"</html>\n";
	HttpResponse	response(200);
	response.AddHeader("Content-Type", "text/html");
	response.SetBody(body.str());
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}
