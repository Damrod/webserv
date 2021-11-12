#include <CGI.hpp>

std::string CGI::GetExecutable_(const std::string &extension) {
	if (requestConfig_->HasCGI(extension)) {
		return requestConfig_->GetCGIBin(extension);
	}
	throw std::invalid_argument(
		"There is no CGI handler for the requested file");
}

CGI::CGI(const HttpRequest &request, const RequestConfig &location,
		 const std::string &extension  // we need the GetExtension
									   // function somewhere common
	) :
	execRet_(0),
	request_(request),
	requestConfig_(&location),
	reqBody_(request.GetBody()),
	arg_path_(requestConfig_->GetRoot() + request.GetPath()),
	exec_path_(GetExecutable_(extension)),
	CGIenvMap_(MakeEnv_()),
	CGIenv_(MakeCEnv_()) {
	pipes_[0] = -1;
	pipes_[1] = -1;
	pipes2_[0] = -1;
	pipes2_[1] = -1;
}

CGI::~CGI(void) {
	for (unsigned int i = 0; CGIenv_[i]; ++i) {
		delete [] CGIenv_[i];
	}
	delete [] CGIenv_;
	CloseAssign_(&pipes_[0]);
	CloseAssign_(&pipes_[1]);
	CloseAssign_(&pipes2_[0]);
	CloseAssign_(&pipes2_[1]);
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
		size_t colonPos = NextStatementThrowing_(statement, ":", true, __LINE__);
		parsedHeaders_.insert(std::make_pair(TrimString(statement.substr(0, colonPos), " "),
							TrimString(statement.substr(colonPos + 1), " ")));
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
	SetHeaders_();
}

void CGI::ExecuteCGI(void) {
	SyscallWrap::pipeWr(pipes_);
	SyscallWrap::pipeWr(pipes2_);
	pid_t pid = SyscallWrap::forkWr();
	if (pid == 0) {
		try {
			CloseAssign_(&pipes2_[1]);
			SyscallWrap::dup2Wr(pipes2_[0], STDIN_FILENO);
			CloseAssign_(&pipes2_[0]);
			SyscallWrap::dup2Wr(pipes_[1], STDOUT_FILENO);
			CloseAssign_(&pipes_[0]);
			CloseAssign_(&pipes_[1]);
			char * const argv[] = {DuplicateString(exec_path_),
									DuplicateString(arg_path_),
									NULL};
			SyscallWrap::execveWr(exec_path_.c_str(), argv, CGIenv_);
		}
		catch (const std::exception &) {
			std::exit(EXIT_FAILURE);
		}
	} else {
		WriteAll_(pipes2_[1], reqBody_.c_str(), reqBody_.size());
		CloseAssign_(&pipes2_[0]);
		CloseAssign_(&pipes2_[1]);
		int status;
		SyscallWrap::waitpidWr(-1, &status, 0);
		if (WIFEXITED(status))
			execRet_ = WEXITSTATUS(status);
		CloseAssign_(&pipes_[1]);
		char buffer[BUFFER_SIZE];
		int bytes_read;
		while ((bytes_read = SyscallWrap::readWr(pipes_[0], buffer,
												 BUFFER_SIZE - 1)) > 0) {
			buffer[bytes_read] = '\0';
			CGIout_ += buffer;
		}
		CloseAssign_(&pipes_[0]);
	}
	ParseCGIOut_();
}

int		CGI::GetExecReturn() const {
	return execRet_;
}

void	CGI::WriteAll_(int fd, const void *buf, size_t count) {
	while (count) {
		count -= SyscallWrap::writeWr(fd, buf, count);
	}
}

void	CGI::CloseAssign_(int *fd) {
	if (*fd != -1) {
		SyscallWrap::closeWr(*fd);
		*fd = -1;
	}
}

std::string  CGI::GetBody() const {
	return CGIoutBody_;
}

std::map<std::string, std::string>	CGI::GetHeaders() const {
	return parsedHeaders_;
}
