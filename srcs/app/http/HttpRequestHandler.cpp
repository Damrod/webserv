#include <HttpRequestHandler.hpp>

HttpRequestHandler::HttpRequestHandler(const ServerConfig &server_config)
	: server_config_(server_config), keep_alive_(true),
	request_location_(NULL), cgi_output_fd_(-1) {
}

HttpRequestHandler::~HttpRequestHandler() {}

std::string	HttpRequestHandler::BuildResponse(IRequest *request) {
	HandleRequest_(dynamic_cast<const HttpRequest *>(request));
	return raw_response_;
}

bool		HttpRequestHandler::GetKeepAlive() const {
	return keep_alive_;
}

bool		HttpRequestHandler::IsCgi() const {
	return cgi_output_fd_ != -1;
}

int			HttpRequestHandler::GetCgiOutputFd() const {
	return cgi_output_fd_;
}

void		HttpRequestHandler::SetKeepAlive_(const HttpRequest &request) {
	if (request.HasHeader("Connection") &&
			ToLowerString(request.GetHeaderValue("Connection")) == "close") {
		keep_alive_ = false;
	}
}

void		HttpRequestHandler::DoRedirection_() {
	const std::size_t status_code = request_location_ ?
									request_location_->common.return_status :
									server_config_.common.return_status;
	const std::string return_url = request_location_ ?
									request_location_->common.return_url :
									server_config_.common.return_url;
	HttpResponse response(status_code);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	response.AddHeader("Location", return_url);
	const std::string body = DefaultResponseBody_(status_code);
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

void		HttpRequestHandler::HandleRequest_(const HttpRequest *request) {
	if (!server_config_.common.return_url.empty()) {
		DoRedirection_();
		return;
	}
	if (request == NULL || !request->IsComplete()) {
		keep_alive_ = false;
		RequestError_(400);
		return;
	}
	SetKeepAlive_(*request);
	request_location_ = new RequestLocation(server_config_, request->GetPath());
	if (!request_location_->common.return_url.empty()) {
		DoRedirection_();
	} else if (HasAcceptedFormat_(*request)) {
		HandleMethod_(*request);
	}
	delete request_location_;
	request_location_ = NULL;
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

void	HttpRequestHandler::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_) {
		response->AddHeader("Connection", "keep-alive");
	} else {
		response->AddHeader("Connection", "close");
	}
	response->AddHeader("Date", CurrentDate());
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
	const CommonConfig &common_cfg = request_location_ != NULL ?
								request_location_->common :
								server_config_.common;
	CommonConfig::ErrorPagesMap::const_iterator it =
										common_cfg.error_pages.find(error_code);
	if (it == common_cfg.error_pages.end()) {
		DefaultStatusResponse_(error_code);
	} else {
		const std::string error_page_path = common_cfg.root + it->second;
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
		if (IsDirectory(full_path_name)) {
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
	const std::string full_path = request_location_->common.root + request_path;
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

bool	HttpRequestHandler::HasAcceptedFormat_(const HttpRequest &request) {
	if (request_location_->HasLocation() &&
									!request_location_->limit_except->empty()) {
		if (std::find(request_location_->limit_except->begin(),
				request_location_->limit_except->end(),
				request.GetMethod()) ==
									request_location_->limit_except->end()) {
			RequestError_(405);
			return false;
		}
	}
	if (request.GetBody().size() >
							request_location_->common.client_max_body_size) {
		RequestError_(413);
		return false;
	}
	if (request.HasHeader("Content-Encoding")) {
		RequestError_(415);
		return false;
	}
	return true;
}

void	HttpRequestHandler::ServeFile_(const std::string &file_path) {
	if (!IsRegularFile(file_path)) {
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
	response.AddHeader("Content-Type", GetMimeType(file_path));
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

void	HttpRequestHandler::DoGet_(const HttpRequest &request) {
	const std::string full_path =
							request_location_->common.root + request.GetPath();
	if (!IsValidPath(full_path)) {
		PathError_();
		return;
	}
	if (IsRegularFile(full_path)) {
		if (IsCgiPath_(full_path)) {
			ExecuteCGI_(request);
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
									full_path + request_location_->common.index;
		if (!request_location_->common.autoindex ||
												IsRegularFile(index_path)) {
			ServeFile_(index_path);
		} else {
			ListDirectory_(request.GetPath());
		}
	}
}

bool	HttpRequestHandler::IsCgiPath_(const std::string &full_path) const {
	const std::string extension = PathExtension(full_path);
	return request_location_->common.cgi_assign.count(extension) > 0;
}

bool	HttpRequestHandler::IsUploadEnabled_() const {
	return !request_location_->common.upload_store.empty();
}

bool	HttpRequestHandler::IsValidUploadPath_(const std::string &path) const {
	if (request_location_->HasLocation()) {
		return path == *request_location_->path;
	}
	return path == "/";
}

void	HttpRequestHandler::UploadFile_(const HttpRequest &request) {
	try {
		FormFile form_file(request);
		const std::string full_upload_path =
								request_location_->common.upload_store +
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

void	HttpRequestHandler::ExecuteCGI_(const HttpRequest &request) {
	keep_alive_ = false;
	try {
		HttpResponse response(200);
		AddCommonHeaders_(&response);
		raw_response_ = response.CreateResponseString(false);
		CGI engine(request, *request_location_);
		cgi_output_fd_ = engine.ExecuteCGI();
	} catch (const std::exception &e) {
		RequestError_(500);
	}
}

void	HttpRequestHandler::DoPost_(const HttpRequest &request) {
	const std::string request_path = request.GetPath();
	const std::string full_path =
							request_location_->common.root + request_path;
	if (IsRegularFile(full_path)) {
		if (IsCgiPath_(full_path)) {
			ExecuteCGI_(request);
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
