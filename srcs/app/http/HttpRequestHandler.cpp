#include <HttpRequestHandler.hpp>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <ctime>
#include <exception>
#include <sstream>
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
HttpRequestHandler::FindLocation_(const std::string &request_path) {
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
		RequestError_(400);
		return;
	}
	SetKeepAlive_(*request);
	if (request->GetMethod() == "GET")
		DoGet_(*request);
	else if (request->GetMethod() == "POST")
		DoPost_(*request);
	else if (request->GetMethod() == "DELETE")
		DoDelete_(*request);
	else
		RequestError_(501);
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

void	HttpRequestHandler::DefaultErrorPage_(const std::size_t error_code) {
	HttpResponse		response(error_code);
	std::stringstream	ss;
	ss << response.GetStatusCode() << " " << response.GetReasonPhrase();
	const std::string	error_message = ss.str();
	const std::string	body = "<html>\n"
						"<head><title>" + error_message + "</title></head>\n"
						"<body>\n"
						"<center><h1>" + error_message + "</h1></center>\n"
						"<hr><center>webserv</center>\n"
						"</body>\n"
						"</html>\n";
	response.SetBody(body);
	response.AddHeader("Content-Type", "text/html");
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::PathError_() {
	if (errno == ENOENT || errno == ENOTDIR)
		RequestError_(404);
	else if (errno == EACCES)
		RequestError_(403);
	else
		RequestError_(500);
}

void	HttpRequestHandler::ListDirectory_(const std::string &full_path,
											const std::string &request_path) {
	DIR	*dir = opendir(full_path.c_str());
	if (dir == NULL) {
		PathError_();
		return;
	}
	std::stringstream	ss;
	ss << "<html>\n" <<
		"<head><title>Index of " << request_path << "</title></head>\n" <<
		"<body>\n" <<
		"<h1>Index of " << request_path <<
		"</h1><hr><pre><a href=\"../\">../</a>\n";
	struct dirent		*entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name == "." || name == ".." || name.rfind(".", 0) == 0)
			continue;
		const std::string full_path_name = full_path + "/" + name;
		struct stat	s;
		if (stat(full_path_name.c_str(), &s) == 0) {
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				name.append("/");
		} else {
			PathError_();
			return;
		}
		ss << "<a href=\"" << name << "\">" << name << "</a>\n";
	}
	closedir(dir);
	ss << "</pre><hr></body>\n" <<
		"</html>\n";
	HttpResponse	response(200);
	response.AddHeader("Content-Type", "text/html");
	response.SetBody(ss.str());
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
}

void	HttpRequestHandler::RequestError_(std::size_t error_code) {
	// TODO(any) Respond with the error page
	//           if its defined in server configuration
	//           and the page exist
	DefaultErrorPage_(error_code);
}

void	HttpRequestHandler::DoGet_(const HttpRequest &request) {
	// TODO(any) Implement GET
	//           Find best matching location if any
	//           Concatenate root + request.path

	// Temporary, testing ListDirectory_
	ListDirectory_(server_config_.common.root, request.GetPath());
}

void	HttpRequestHandler::DoPost_(const HttpRequest &request) {
	// TODO(any) Implement POST
	(void)request;
	HttpResponse	response(200);
	std::string		body = "Responding to a POST request\n";

	response.AddHeader("Content-Type", "text/plain");
	response.SetBody(body);
	AddCommonHeaders_(&response);
	raw_response_ = response.CreateResponseString();
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
