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
#include <stdexcept>
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

void		HttpRequestHandler::HandleRequest_() {
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
HttpRequestHandler::DefaultResponseBody_(const std::string &message) const {
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
	std::stringstream	ss;
	ss << response.GetStatusCode() << " " << response.GetReasonPhrase();
	response.SetBody(DefaultResponseBody_(ss.str()));
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

void	HttpRequestHandler::AddDirectoryContent_(std::stringstream *ss,
											const Location *location,
											const std::string &full_path) {
	DIR *dir = opendir(full_path.c_str());
	if (dir == NULL) {
		PathError_(location);
		throw std::invalid_argument("invalid directory");
	}
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name.rfind(".", 0) == 0)
			continue;
		const std::string full_path_name = full_path + "/" + name;
		struct stat statbuf;
		if (stat(full_path_name.c_str(), &statbuf) == 0) {
			if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
				name.append("/");
		} else {
			closedir(dir);
			PathError_(location);
			throw std::runtime_error("stat error");
		}
		*ss << "<a href=\"" << name << "\">" << name << "</a>\n";
	}
	closedir(dir);
}

void	HttpRequestHandler::ListDirectory_(const Location *location,
											const std::string &request_path) {
	std::stringstream ss;
	ss << "<html>\n" <<
		"<head><title>Index of " << request_path << "</title></head>\n" <<
		"<body>\n" <<
		"<h1>Index of " << request_path <<
		"</h1><hr><pre><a href=\"../\">../</a>\n";
	try {
		const std::string full_path = GetFullPath_(location, request_path);
		AddDirectoryContent_(&ss, location, full_path);
	}
	catch (const std::exception &) {
		return;
	}
	ss << "</pre><hr></body>\n" <<
		"</html>\n";
	HttpResponse	response(200);
	response.AddHeader("Content-Type", "text/html");
	response.SetBody(ss.str());
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

void	HttpRequestHandler::MovedPermanently_(const HttpRequest &request,
												const std::string &index_path) {
	HttpResponse response(301);
	AddCommonHeaders_(&response);
	response.AddHeader("Content-Type", GetMimeType_(index_path));
	std::stringstream ss;
	ss << "http://" << request.GetHost() << ":" << request.GetPort() <<
		request.GetPath() << "/";
	response.AddHeader("Location", ss.str());
	// TODO(any) Set Content-Length to the size of the file
	raw_response_ = response.CreateResponseString();
}

std::string	HttpRequestHandler::GetMimeType_(const std::string &path) const {
	return kMimeTypes_.GetMimeType(PathExtension_(path));
}

void	HttpRequestHandler::DoGet_(const Location *location,
									const HttpRequest &request) {
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
				MovedPermanently_(request, index_path);
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
	if (stat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFDIR)
		return true;
	return false;
}

bool	HttpRequestHandler::IsRegularFile_(const std::string &path) const {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) == 0 &&
			(statbuf.st_mode & S_IFMT) == S_IFREG)
		return true;
	return false;
}
