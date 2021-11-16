#include <CGI.hpp>

std::string CGI::GetExecutable_(const std::string &extension) {
	if (request_location_->common.cgi_assign.count(extension) > 0) {
			return request_location_->
				common.cgi_assign.find(extension)->second;
		}
	throw std::invalid_argument("There is no CGI handler for the "
									"requested file");
}

CGI::CGI(const HttpRequest &request, const RequestLocation &location) :
	request_(request),
	request_location_(&location),
	arg_path_(request_location_->common.root + request.GetPath()),
	exec_path_(GetExecutable_(PathExtension(arg_path_))),
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
	env_.insert(std::make_pair("CONTENT_LENGTH",
									request_.GetHeaderValue("Content-Length")));
	env_.insert(std::make_pair("CONTENT_TYPE",
									request_.GetHeaderValue("Content-Type")));
	env_.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	env_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	env_.insert(std::make_pair("SERVER_PORT",
											ValueToString(request_.GetPort())));
	env_.insert(std::make_pair("QUERY_STRING", request_.GetQueryString()));
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

int CGI::ExecuteCGI(void) {
	FILE *fp = std::tmpfile();
	if (!fp) {
		throw std::runtime_error(std::strerror(errno));;
	}
	const std::string body = request_.GetBody();
	std::fwrite(body.c_str(), 1, body.size(), fp);
	std::rewind(fp);
	SyscallWrap::pipeWr(fds_);
	if (!IsExecutable(exec_path_) || !IsExecutable(arg_path_)) {
		throw std::runtime_error(std::strerror(errno));;
	}
	pid_t pid = SyscallWrap::forkWr();
	if (pid == 0) {
		std::signal(SIGCHLD, SIG_IGN);
		std::signal(SIGPIPE, SIG_DFL);
		try {
			CloseAssign_(&fds_[0]);
			int cgi_input = fileno(fp);
			SyscallWrap::dup2Wr(cgi_input, STDIN_FILENO);
			SyscallWrap::dup2Wr(fds_[1], STDOUT_FILENO);
			CloseAssign_(&fds_[1]);

			char * const argv[] = {DuplicateString(exec_path_),
									DuplicateString(arg_path_),
									NULL};
			SyscallWrap::execveWr(exec_path_.c_str(), argv, CGIenv_);
		}
		catch (const std::exception &) {
			std::exit(EXIT_FAILURE);
		}
	}
	if (std::fclose(fp)) {
		throw std::runtime_error(std::strerror(errno));;
	}
	CloseAssign_(&fds_[1]);
	int cgi_output_fd = SyscallWrap::dupWr(fds_[0]);
	return cgi_output_fd;
}

void	CGI::CloseAssign_(int *fd) {
	if (*fd != -1) {
		SyscallWrap::closeWr(*fd);
		*fd = -1;
	}
}
