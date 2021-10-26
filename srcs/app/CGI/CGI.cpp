#include <CGI.hpp>

std::string CGI::GetExecutable_(const std::string &extension) {
	if (request_location_->common.cgi_assign.count(extension) > 0) {
			return request_location_->
				common.cgi_assign.find(extension)->second;
		}
	throw std::invalid_argument("There is no CGI handler for the "
									"requested file");
}

CGI::CGI(const HttpRequest &request, const RequestLocation &location,
		 const std::string &extension,  // we need the GetExtension
									   // function somewhere common
		 HttpResponse *response
	) :
	execRet_(0),
	request_(request),
	request_location_(&location),
	reqBody_(request.GetBody()),
	arg_path_(request_location_->common.root + request.GetPath()),
	exec_path_(GetExecutable_(extension)),
	response_(response),
	CGIenvMap_(MakeEnv_()),
	CGIenv_(MakeCEnv_())
{}

CGI::~CGI(void) {
	for (unsigned int i = 0; CGIenv_[i]; ++i) {
		delete [] CGIenv_[i];
	}
	delete [] CGIenv_;
}

std::map<std::string, std::string> CGI::MakeEnv_(void) {
	// std::map<std::string, std::string>	headers_ = request_.GetHeaders();
	std::map<std::string, std::string>	env_;

	env_.insert(std::make_pair("REDIRECT_STATUS", "200"));
	env_.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	env_.insert(std::make_pair("REQUEST_METHOD", request_.GetMethod()));
	env_.insert(std::make_pair("CONTENT_TYPE",
									 request_.GetHeaderValue("Content-Type")));
	env_.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	env_.insert(std::make_pair("SERVER_SOFTWARE", "webserv/1.0"));
	env_.insert(std::make_pair("SERVER_PORT",
									 UInt16ToStr(request_.GetPort())));
	return env_;
}

char **CGI::MakeCEnv_(void) {
	char **env_ = new char*[CGIenvMap_.size() + 1];
	size_t i = 0;
	std::map<std::string, std::string>::const_iterator it = CGIenvMap_.begin();
	for (; it != CGIenvMap_.end(); ++it, ++i) {
		env_[i] = StrDupWrapper_(it->first + "=" + it->second);
	}
	env_[i] = NULL;
	return env_;
}

char *CGI::StrDupWrapper_(const std::string &str) {
	char *element = new char[str.size() + 1];
	memcpy(element, str.c_str(), str.size());
	element[str.size()] = '\0';
	return element;
}

size_t CGI::NextStatementThrowing_(const std::string &str,
								   const std::string &separator,
								   bool needsSeparator,
								   size_t line) {
	std::ostringstream strline;
	strline << line;
	size_t next_statement_delimiter = str.find(separator);
	if (next_statement_delimiter == std::string::npos)
		next_statement_delimiter = str.size();
	if (needsSeparator && str.find(separator) == std::string::npos)
		throw std::invalid_argument("Invalid header format for str: \"" + str +
									"\" and separator: \"" + separator + "\""
									" in line:" + strline.str());
	return next_statement_delimiter;
}

void CGI::SetHeaders_(void) {
	std::string remain = CGIoutHeaders_;
	while(!remain.empty()) {
		size_t next_statement_delimiter = NextStatementThrowing_(remain,
																 kCRLF_,
																 false,
																 __LINE__);
		std::string statement = remain.substr(0, next_statement_delimiter);
		size_t colonPos = NextStatementThrowing_(remain, ":", true, __LINE__);
		response_->AddHeader(TrimString(statement.substr(0, colonPos), " "),
							TrimString(statement.substr(colonPos + 1), " "));
		size_t remain_delimiter = NextStatementThrowing_(remain, kCRLF_, false,
							 __LINE__) + std::string(kCRLF_).size();
		remain = remain.substr(remain_delimiter > remain.size() ?
							   remain.size() : remain_delimiter);
	}
}

const char CGI::kCRLF_[] = "\r\n";

void CGI::ParseCGIOut_(void) {
	size_t headers_end;
	if ((headers_end = CGIout_.find(std::string(kCRLF_) + kCRLF_))
		== std::string::npos)
		throw std::invalid_argument("Invalid CGI request headers");
	CGIoutHeaders_ = CGIout_.substr(0, headers_end);
	CGIoutBody_ = CGIout_.substr(headers_end + (std::string(kCRLF_)
												+ kCRLF_).size());
	response_->SetBody(CGIoutBody_);
	SetHeaders_();
}

void CGI::ExecuteCGI(void) {
	int pipes[2];
	int pipes2[2];
	SyscallWrap::pipeWr(pipes);
	SyscallWrap::pipeWr(pipes2);
	pid_t pid = SyscallWrap::forkWr();
	if (pid == 0) {
		SyscallWrap::closeWr(pipes2[1]);
		SyscallWrap::dup2Wr(pipes2[0], STDIN_FILENO);
		SyscallWrap::closeWr(pipes2[0]);
		SyscallWrap::dup2Wr(pipes[1], STDOUT_FILENO);
		SyscallWrap::closeWr(pipes[0]);
		SyscallWrap::closeWr(pipes[1]);
		char * const argv[] = {StrDupWrapper_(exec_path_),
								StrDupWrapper_(arg_path_),
								NULL};
		SyscallWrap::execveWr(exec_path_.c_str(), argv, CGIenv_);
		std::exit(EXIT_FAILURE);
	} else {
		WriteAll_(pipes2[1], reqBody_.c_str(), reqBody_.size());
		SyscallWrap::closeWr(pipes2[0]);
		SyscallWrap::closeWr(pipes2[1]);
		int status;
		SyscallWrap::waitpidWr(-1, &status, 0);
		if (WIFEXITED(status))
			execRet_ = WEXITSTATUS(status);
		SyscallWrap::closeWr(pipes[1]);
		char buffer[BUFFER_SIZE];
		int bytes_read;
		while ((bytes_read = SyscallWrap::readWr(pipes[0], buffer,
												 BUFFER_SIZE - 1)) > 0) {
			buffer[bytes_read] = '\0';
			CGIout_ += buffer;
		}
		SyscallWrap::closeWr(pipes[0]);
	}
	ParseCGIOut_();
}

void	CGI::WriteAll_(int fd, const void *buf, size_t count) {
	while (count) {
		count -= SyscallWrap::writeWr(fd, buf, count);
	}
}

ssize_t CGI::SyscallWrap::writeWr(int fd, const void *buf, size_t count) {
	ssize_t ret;
	if ((ret = write(fd, buf, count)) == -1)
		throw std::runtime_error("Problem writing");
	return ret;
}

ssize_t CGI::SyscallWrap::readWr(int fd, void *buf, size_t count) {
	ssize_t ret;
	if ((ret = read(fd, buf, count)) == -1)
		throw std::runtime_error("Problem reading");
	return ret;
}

int CGI::SyscallWrap::pipeWr(int pipefd[2]) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
		throw std::runtime_error(std::strerror(errno));;
	}
	return ret;
}

int CGI::SyscallWrap::dup2Wr(int oldfd, int newfd) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int CGI::SyscallWrap::execveWr(const char *pathname, char *const argv[],
			 char *const envp[]) {
	int ret;
	if ((ret = execve(pathname, argv, envp)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int CGI::SyscallWrap::closeWr(int fd) {
	int ret;
	if ((ret = close(fd)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

pid_t CGI::SyscallWrap::waitpidWr(pid_t pid, int *wstatus, int options) {
	pid_t ret;
	if ((ret = waitpid(pid, wstatus, options)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

pid_t CGI::SyscallWrap::forkWr(void) {
	int ret;
	if ((ret = fork()) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

