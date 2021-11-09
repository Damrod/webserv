#include <HttpRequestHandler.hpp>

HttpRequestHandler::HttpRequestHandler(const ServerConfig &server_config)
	: server_config_(server_config), keep_alive_(true),
		requestConfig_(NULL) {
}

HttpRequestHandler::~HttpRequestHandler() {}

// Response building
std::string	HttpRequestHandler::BuildResponse(IRequest *request) {
	HandleRequest_(dynamic_cast<const HttpRequest *>(request));
	return raw_response_;
}

void		HttpRequestHandler::HandleRequest_(const HttpRequest *request) {
	if (request == NULL || request->GetState() == RequestState::kInvalid) {
		RequestError_(400);
		return;
	}
	SetKeepAlive_(*request);
	// This will be injected from factory
	requestConfig_ = new RequestConfig(server_config_, request->GetPath());
	if (!requestConfig_->getReturnUrl().empty()) {
		DoRedirection_();
	} else if (HasAcceptedFormat_(*request)) {
		HandleMethod_(*request);
	}
	delete requestConfig_;
	requestConfig_ = NULL;
}

void	HttpRequestHandler::HandleMethod_(const HttpRequest &request) {
	const std::string request_method = request.GetMethod();

	if (request_method == "GET") {
		DoGet_(request);
	} else if (request_method == "POST") {
		DoPost_(request);
	} else if (request_method == "DELETE") {
		DoDelete_(request);
	} else {
		RequestError_(501);
	}
}

// ------- Specific Do methods

void		HttpRequestHandler::DoRedirection_() {
	HttpResponse response(requestConfig_->getReturnStatus());
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	response.AddHeader("Location", requestConfig_->getReturnUrl());
	const std::string body = DefaultResponseBody_(requestConfig_->getReturnStatus());
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::DoPost_(const HttpRequest &request) {
	const std::string request_path = request.GetPath();
	const std::string full_path =
							requestConfig_->getRoot() + request_path;
	if (IsRegularFile_(full_path)) {
		if (IsCGI_(full_path)) {
			ExecuteCGI_(request, full_path);
		} else {
			RequestError_(501);
		}
	} else {
		if (IsUploadEnabled_() && IsValidUploadPath_(request_path)) {
			UploadFile_(request);
		} else {
			RequestError_(404);
		}
	}
}

void	HttpRequestHandler::DoDelete_(const HttpRequest &request) {
	// TODO(any) Implement DELETE
	(void)request;
	HttpResponse	response(200);
	std::string		body = "Responding to a DELETE request\n";

	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(body);
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::DoGet_(const HttpRequest &request) {
	const std::string full_path =
							requestConfig_->getRoot() + request.GetPath();
	if (!IsValidPath_(full_path)) {
		PathError_();
		return;
	}
	if (IsRegularFile_(full_path)) {
		if (IsCGI_(full_path)) {
			ExecuteCGI_(request, full_path);
		} else {
			ServeFile_(full_path);
		}
	} else {
		const bool has_end_slash = full_path[full_path.size() - 1] == '/';
		if (!has_end_slash) {
			MovedPermanently_(request);
			return;
		}
		const std::string index_path =
									full_path + requestConfig_->getIndex();
		if (requestConfig_->hasAutoindex() && !IsRegularFile_(index_path)) {
			ListDirectory_(request.GetPath());
		} else {
			ServeFile_(index_path);
		}
	}
}

// Setup Tooling

bool	HttpRequestHandler::HasAcceptedFormat_(const HttpRequest &request) {
	if (requestConfig_->limits(request.GetMethod())) {
			RequestError_(405);
			return false;
	}
	if (request.GetBody().size() >
							requestConfig_->getClientMaxBodySize()) {
		RequestError_(413);
		return false;
	}
	if (request.HasHeader("Content-Encoding")) {
		RequestError_(415);
		return false;
	}
	return true;
}

bool		HttpRequestHandler::GetKeepAlive() const {
	return keep_alive_;
}

void		HttpRequestHandler::SetKeepAlive_(const HttpRequest &request) {
	if (request.HasHeader("Connection") &&
			ToLowerString(request.GetHeaderValue("Connection")) == "close") {
		keep_alive_ = false;
	}
}

void	HttpRequestHandler::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate_());
}

std::string
HttpRequestHandler::DefaultResponseBody_(const std::size_t status_code) const {
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

void
HttpRequestHandler::DefaultStatusResponse_(const std::size_t status_code) {
	HttpResponse		response(status_code);
	response.SetBody(DefaultResponseBody_(status_code));
	response.AddHeader("Content-Type", "text/html");
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::RequestError_(const std::size_t error_code) {
	const std::string error_page_path = requestConfig_->getErrorPagePath(error_code);

	if (error_page_path.empty()) {
		DefaultStatusResponse_(error_code);
	} else {
		ServeFile_(error_page_path);
	}
}

void	HttpRequestHandler::PathError_() {
	if (errno == ENOENT || errno == ENOTDIR) {
		RequestError_(404);
	} else if (errno == EACCES) {
		RequestError_(403);
	} else {
		RequestError_(500);
	}
}

bool	HttpRequestHandler::TryAddDirectoryContent_(std::stringstream *body,
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

void	HttpRequestHandler::ListDirectory_(const std::string &request_path) {
	std::stringstream body;
	body << "<html>\n" <<
		"<head><title>Index of " << request_path << "</title></head>\n" <<
		"<body>\n" <<
		"<h1>Index of " << request_path <<
		"</h1><hr><pre><a href=\"../\">../</a>\n";
	const std::string full_path = requestConfig_->getRoot() + request_path;
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

void	HttpRequestHandler::ServeFile_(const std::string &file_path) {
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

void	HttpRequestHandler::MovedPermanently_(const HttpRequest &request) {
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

void	HttpRequestHandler::UploadFile_(const HttpRequest &request) {
	try {
		FormFile form_file(request);
		const std::string full_upload_path =
								requestConfig_->getUploadStore() +
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

void	HttpRequestHandler::ExecuteCGI_(const HttpRequest &request,
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
std::string	HttpRequestHandler::PathExtension_(const std::string &path) const {
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

std::string	HttpRequestHandler::GetMimeType_(const std::string &path) const {
	return MimeTypes::GetMimeType(PathExtension_(path));
}

std::string	HttpRequestHandler::CurrentDate_() const {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

// ------- Tooling methods

bool	HttpRequestHandler::IsValidPath_(const std::string &path) const {
	struct stat statbuf;
	return stat(path.c_str(), &statbuf) == 0;
}

bool	HttpRequestHandler::IsDirectory_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFDIR) {
		return true;
	}
	return false;
}

bool	HttpRequestHandler::IsRegularFile_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFREG) {
		return true;
	}
	return false;
}

bool	HttpRequestHandler::IsCGI_(const std::string &full_path) const {
	return requestConfig_->hasCGI(PathExtension_(full_path)) > 0;
}

bool	HttpRequestHandler::IsUploadEnabled_() const {
	return !requestConfig_->getUploadStore().empty();
}

bool	HttpRequestHandler::IsValidUploadPath_(const std::string &path) const {
	return path == requestConfig_->getPath();
}
