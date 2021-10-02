#include <HttpRequestHandler.hpp>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <ctime>
#include <algorithm>
#include <exception>
#include <fstream>
#include <sstream>
#include <HttpStatusCodes.hpp>
#include <StringUtils.hpp>

HttpRequestHandler::HttpRequestHandler(const ServerConfig &server_config,
										const std::string &raw_request)
	: server_config_(server_config), raw_request_(raw_request),
		keep_alive_(true) {
	HandleRequest_();
}

std::string	HttpRequestHandler::GetRawResponse() const {
	return raw_response_;
}

bool		HttpRequestHandler::GetKeepAlive() const {
	return keep_alive_;
}

void		HttpRequestHandler::SetKeepAlive_(const HttpRequest &request) {
	if (request.HasHeader("Connection") &&
			ToLowerString(request.GetHeaderValue("Connection")) == "close")
		keep_alive_ = false;
}

const Location*
HttpRequestHandler::FindLocation_(const std::string &request_path) const {
	const std::vector<Location>	&locations = server_config_.locations;
	if (locations.empty())
		return NULL;
	ssize_t	best_match_idx = -1;
	for (std::size_t i = 0; i < locations.size(); ++i) {
		if (request_path.rfind(locations[i].path, 0) != 0)
			continue;
		if (best_match_idx == -1 ||
			locations[best_match_idx].path.size() < locations[i].path.size()) {
			best_match_idx = i;
		}
	}
	return best_match_idx == -1 ? NULL : &locations[best_match_idx];
}

std::string	HttpRequestHandler::GetReturnUrl_(const Location *location) const {
	if (location)
		return location->common.return_url;
	return server_config_.common.return_url;
}

std::size_t
HttpRequestHandler::GetReturnStatus_(const Location *location) const {
	if (location)
		return location->common.return_status;
	return server_config_.common.return_status;
}

void		HttpRequestHandler::DoRedirection_(const Location *location) {
	const std::size_t status_code = GetReturnStatus_(location);
	HttpResponse response(status_code);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", "text/html");
	response.AddHeader("Location", GetReturnUrl_(location));
	const std::string body = DefaultResponseBody_(status_code);
	response.SetBody(body);
	raw_response_ = response.CreateResponseString();
}

void		HttpRequestHandler::HandleRequest_() {
	if (!server_config_.common.return_url.empty()) {
		DoRedirection_(NULL);
		return;
	}
	HttpRequest	*request = NULL;
	try {
		request = new HttpRequest(raw_request_);
	}
	catch (const std::exception &e) {
		RequestError_(NULL, 400);
		return;
	}
	SetKeepAlive_(*request);
	const Location *location = FindLocation_(request->GetPath());
	if (request->GetMethod() == "GET")
		DoGet_(location, *request);
	else if (request->GetMethod() == "POST")
		DoPost_(location, *request);
	else if (request->GetMethod() == "DELETE")
		DoDelete_(location, *request);
	else
		RequestError_(location, 501);
	delete request;
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

void	HttpRequestHandler::AddCommonHeaders_(HttpResponse *response) {
	response->AddHeader("Server", "webserv");
	if (keep_alive_)
		response->AddHeader("Connection", "keep-alive");
	else
		response->AddHeader("Connection", "close");
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

void	HttpRequestHandler::PathError_(const Location *location) {
	if (errno == ENOENT || errno == ENOTDIR)
		RequestError_(location, 404);
	else if (errno == EACCES)
		RequestError_(location, 403);
	else
		RequestError_(location, 500);
}

std::string	HttpRequestHandler::GetFullPath_(const Location *location,
										const std::string &request_path) const {
	if (!location)
		return server_config_.common.root + request_path;
	return location->common.root + request_path;
}

bool	HttpRequestHandler::TryAddDirectoryContent_(std::stringstream *body,
											const Location *location,
											const std::string &full_path) {
	DIR *dir = opendir(full_path.c_str());
	if (dir == NULL) {
		PathError_(location);
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name.rfind(".", 0) == 0)
			continue;
		const std::string full_path_name = full_path + "/" + name;
		if (IsDirectory_(full_path_name))
				name.append("/");
		*body << "<a href=\"" << name << "\">" << name << "</a>\n";
	}
	closedir(dir);
	return true;
}

void	HttpRequestHandler::ListDirectory_(const Location *location,
											const std::string &request_path) {
	std::stringstream body;
	body << "<html>\n" <<
		"<head><title>Index of " << request_path << "</title></head>\n" <<
		"<body>\n" <<
		"<h1>Index of " << request_path <<
		"</h1><hr><pre><a href=\"../\">../</a>\n";
	const std::string full_path = GetFullPath_(location, request_path);
	if (!TryAddDirectoryContent_(&body, location, full_path))
		return;
	body << "</pre><hr></body>\n" <<
		"</html>\n";
	HttpResponse	response(200);
	response.AddHeader("Content-Type", "text/html");
	response.SetBody(body.str());
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::RequestError_(const Location *location,
											const std::size_t error_code) {
	// TODO(any) Respond with the error page
	//           if its defined in server configuration
	//           and the page exist
	(void)location;
	DefaultStatusResponse_(error_code);
}

const CommonConfig &
HttpRequestHandler::GetCommonConfig(const Location *location) const {
	if (location)
		return location->common;
	return server_config_.common;
}

bool	HttpRequestHandler::HasAcceptedFormat_(const Location *location,
												const HttpRequest &request) {
	if (location && !location->limit_except.empty()) {
		if (std::find(location->limit_except.begin(),
					location->limit_except.end(),
					request.GetMethod()) == location->limit_except.end()) {
			RequestError_(location, 405);
			return false;
		}
	}
	const CommonConfig &cfg = GetCommonConfig(location);
	if (request.GetBody().size() > cfg.client_max_body_size) {
		RequestError_(location, 413);
		return false;
	}
	if (request.HasHeader("Content-Encoding")) {
		RequestError_(location, 415);
		return false;
	}
	return true;
}

std::string	HttpRequestHandler::PathExtension_(const std::string &path) const {
	const std::size_t extension_position = path.rfind(".");
	if (extension_position == std::string::npos || extension_position < 2)
		return "";
	const std::size_t	last_dir_position = path.rfind("/");
	if (last_dir_position == std::string::npos) {
		return path.substr(extension_position + 1);
	}
	if (last_dir_position < extension_position - 1) {
		const std::string last_path_part = path.substr(last_dir_position + 1);
		if (last_path_part != "." && last_path_part != "..")
			return path.substr(extension_position + 1);
	}
	return "";
}

void	HttpRequestHandler::ServeFile_(const Location *location,
												const std::string &file_path) {
	std::ifstream ifs(file_path.c_str(), std::ios::in | std::ios::binary);
	if (!ifs) {
		RequestError_(location, 404);
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

std::string	HttpRequestHandler::GetMimeType_(const std::string &path) const {
	return MimeTypes::GetMimeType(PathExtension_(path));
}

void	HttpRequestHandler::DoGet_(const Location *location,
									const HttpRequest &request) {
	if (location && !location->common.return_url.empty()) {
		DoRedirection_(location);
		return;
	}
	if (!HasAcceptedFormat_(location, request)) {
		return;
	}
	const std::string full_path = GetFullPath_(location, request.GetPath());
	if (!IsValidPath_(full_path)) {
		PathError_(location);
		return;
	}
	if (IsDirectory_(full_path)) {
		const CommonConfig &cfg = GetCommonConfig(location);
		const bool has_end_slash = full_path[full_path.size() - 1] == '/';
		std::string index_path;
		if (has_end_slash)
			index_path = full_path + cfg.index;
		else
			index_path = full_path + "/" + cfg.index;
		if (!cfg.autoindex || IsValidPath_(index_path)) {
			if (has_end_slash)
				ServeFile_(location, index_path);
			else
				MovedPermanently_(request);
		} else {
			ListDirectory_(location, request.GetPath());
		}
	} else {
		ServeFile_(location, full_path);
	}
}

void	HttpRequestHandler::DoPost_(const Location *location,
									const HttpRequest &request) {
	// TODO(any) Implement POST
	(void)request;
	(void)location;
	HttpResponse	response(200);
	std::string		body = "Responding to a POST request\n";

	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(body);
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::DoDelete_(const Location *location,
										const HttpRequest &request) {
	// TODO(any) Implement DELETE
	(void)request;
	(void)location;
	HttpResponse	response(200);
	std::string		body = "Responding to a DELETE request\n";

	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(body);
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

bool	HttpRequestHandler::IsValidPath_(const std::string &path) const {
	struct stat statbuf;
	return stat(path.c_str(), &statbuf) == 0;
}

bool	HttpRequestHandler::IsDirectory_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFDIR)
		return true;
	return false;
}

bool	HttpRequestHandler::IsRegularFile_(const std::string &path) const {
	struct stat statbuf;
	if (lstat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFREG)
		return true;
	return false;
}
