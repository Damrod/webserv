#include <CGI.hpp>

std::string CGI::GetExecutable_() {
	const std::string extension = file_.GetPathExtension();
	if (requestConfig_->HasCGI(extension)) {
		return requestConfig_->GetCGIBin(extension);
	}
	throw std::invalid_argument(
		"There is no CGI handler for the requested file");
}

CGI::CGI(const HttpRequest &request, const RequestConfig &location,
		const File &file
	) :
	request_(request),
	requestConfig_(&location),
	file_(file),
	client_address_(MakeClientAddress_()),
	arg_path_(file_.GetPath()),
	exec_path_(GetExecutable_()),
	CGIenvMap_(MakeEnv_()),
	CGIenv_(MakeCEnv_()) {
	fds_[0] = -1;
	fds_[1] = -1;
}

CGI::~CGI(void) {
	for (unsigned int i = 0; CGIenv_[i]; ++i) {
		delete [] CGIenv_[i];
	}
	delete [] CGIenv_;
	CloseAssign_(&fds_[0]);
	CloseAssign_(&fds_[1]);
}

std::map<std::string, std::string> CGI::MakeEnv_(void) {
	std::map<std::string, std::string>	env_;

	env_.insert(std::make_pair("REDIRECT_STATUS", "200"));
	env_.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	env_.insert(std::make_pair("REQUEST_METHOD", request_.GetMethod()));
	env_.insert(std::make_pair("REQUEST_URI", request_.GetRequestTarget()));
	env_.insert(std::make_pair("CONTENT_LENGTH",
									request_.GetHeaderValue("Content-Length")));
	env_.insert(std::make_pair("CONTENT_TYPE",
									request_.GetHeaderValue("Content-Type")));
	env_.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	env_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	env_.insert(std::make_pair("SERVER_PORT",
											ValueToString(request_.GetPort())));
	env_.insert(std::make_pair("SERVER_NAME", request_.GetHost()));
	env_.insert(std::make_pair("QUERY_STRING", request_.GetQueryString()));
	env_.insert(std::make_pair("SCRIPT_FILENAME", arg_path_));
	env_.insert(std::make_pair("SCRIPT_NAME", requestConfig_->GetRequestPath()));
	env_.insert(std::make_pair("PATH_INFO", requestConfig_->GetRequestPathInfo()));
	if (requestConfig_->GetRequestPathInfo().empty()) {
		env_.insert(std::make_pair("PATH_TRANSLATED", ""));
	} else {
		env_.insert(std::make_pair("PATH_TRANSLATED",
			requestConfig_->GetRoot() + requestConfig_->GetRequestPathInfo()));
	}
	env_.insert(std::make_pair("REMOTE_ADDR", GetClientAddress_()));
	env_.insert(std::make_pair("REMOTE_HOST", GetClientAddress_()));

	env_.insert(std::make_pair("HTTP_ACCEPT",
										request_.GetHeaderValue("Accept")));
	env_.insert(std::make_pair("HTTP_HOST", request_.GetHeaderValue("Host")));
	env_.insert(std::make_pair("HTTP_COOKIE",
										request_.GetHeaderValue("Cookie")));
	env_.insert(std::make_pair("HTTP_USER_AGENT",
										request_.GetHeaderValue("User-Agent")));
	return env_;
}

char **CGI::MakeCEnv_(void) {
	char **env_ = new char*[CGIenvMap_.size() + 1];
	size_t i = 0;
	std::map<std::string, std::string>::const_iterator it = CGIenvMap_.begin();
	for (; it != CGIenvMap_.end(); ++it, ++i) {
		env_[i] = DuplicateString(it->first + "=" + it->second);
	}
	env_[i] = NULL;
	return env_;
}

CgiInfo CGI::ExecuteCGI(void) {
	FILE *fp = std::tmpfile();
	if (!fp) {
		throw std::runtime_error(std::strerror(errno));;
	}
	const std::string body = request_.GetBody();
	std::fwrite(body.c_str(), 1, body.size(), fp);
	std::rewind(fp);
	SyscallWrap::pipeWr(fds_ DEBUG_INFO);
	pid_t pid = SyscallWrap::forkWr(__FILE__, __FUNCTION__, __LINE__);
	if (pid == 0) {
		std::signal(SIGPIPE, SIG_DFL);
		try {
			CloseAssign_(&fds_[0]);
			int cgi_input = fileno(fp);
			SyscallWrap::dup2Wr(cgi_input, STDIN_FILENO DEBUG_INFO);
			SyscallWrap::dup2Wr(fds_[1], STDOUT_FILENO DEBUG_INFO);
			CloseAssign_(&fds_[1]);

			char * const argv[] = {DuplicateString(exec_path_),
									DuplicateString(arg_path_),
									NULL};
			SyscallWrap::execveWr(exec_path_.c_str(), argv, CGIenv_ DEBUG_INFO);
		}
		catch (const std::exception &) {
			std::exit(EXIT_FAILURE);
		}
	}
	if (std::fclose(fp)) {
		throw std::runtime_error(std::strerror(errno));;
	}
	CloseAssign_(&fds_[1]);
	int cgi_output_fd = SyscallWrap::dupWr(fds_[0] DEBUG_INFO);
	return CgiInfo(pid, cgi_output_fd,
			requestConfig_->GetErrorPages(),
			requestConfig_->GetRoot());
}

void	CGI::CloseAssign_(int *fd) {
	if (*fd != -1) {
		SyscallWrap::closeWr(*fd DEBUG_INFO);
		*fd = -1;
	}
}

std::string	CGI::MakeClientAddress_() {
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];

	len = sizeof(addr);
	SyscallWrap::getpeernameWr(request_.GetSocket(),
							(struct sockaddr *)&addr, &len DEBUG_INFO);
	struct sockaddr_in *src = (struct sockaddr_in *)&addr;
	SyscallWrap::inet_ntopWr(AF_INET, &src->sin_addr, ipstr, sizeof(ipstr)
	DEBUG_INFO);
	return ipstr;
}

std::string CGI::GetClientAddress_() const {
	return client_address_;
}
