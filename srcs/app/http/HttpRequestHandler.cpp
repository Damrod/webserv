#include <HttpRequestHandler.hpp>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <sstream>
#include <StringUtils.hpp>

HttpRequestHandler::HttpRequestHandler(const ServerConfig &server_config,
										const std::string &raw_request)
	: server_config_(server_config), raw_request_(raw_request),
		keep_alive_(true) {
	HandleRequest_();
	(void)server_config_;
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
	char		buffer[100];
	std::time_t	date = std::time(NULL);
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

void	HttpRequestHandler::DefaultErrorPage_(std::size_t error_code) {
	HttpResponse		response(error_code);
	std::size_t			status_code = response.GetStatusCode();
	std::string			reason_phrase = response.GetReasonPhrase();
	std::stringstream	ss;
	ss << status_code << " " << reason_phrase;
	std::string			error_message = ss.str();
	std::string			body = "<html>\n"
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

std::string	HttpRequestHandler::RealPath_(const std::string &path) {
	char	buffer[PATH_MAX];
	char	*error = realpath(path.c_str(), buffer);
	if (error == NULL)
		return "";
	return buffer;
}

void	HttpRequestHandler::ListDirectory_(const std::string &full_real_path,
											const std::string &request_path) {
	DIR	*dir = opendir(full_real_path.c_str());
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
		std::string dir_name = entry->d_name;
		if (dir_name == "." || dir_name == ".." || dir_name.rfind(".", 0) == 0)
			continue;
		const std::string full_dir_path = full_real_path + "/" + dir_name;
		struct stat	s;
		if (stat(full_dir_path.c_str(), &s) == 0) {
			if ((s.st_mode & S_IFMT) == S_IFDIR)
				dir_name.append("/");
		} else {
			PathError_();
			return;
		}
		ss << "<a href=\"" << dir_name << "\">" << dir_name << "</a>\n";
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
	const std::string real_path = RealPath_(server_config_.common.root);
	if (real_path.empty())
		PathError_();
	else
		ListDirectory_(real_path, request.GetPath());
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
