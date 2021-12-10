#include <SyscallWrap.hpp>

int SyscallWrap::pipeWr(int pipefd[2] DEBUG_ARGS) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
		ThrowException_("pipe" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::dupWr(int oldfd DEBUG_ARGS) {
	int ret;
	if ((ret = dup(oldfd)) == -1) {
		ThrowException_("dup" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::dup2Wr(int oldfd,
						int newfd DEBUG_ARGS) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1) {
		ThrowException_("dup2" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::execveWr(const char *pathname,
						  char *const argv[],
						  char *const envp[] DEBUG_ARGS) {
	int ret;
	if ((ret = execve(pathname, argv, envp)) == -1) {
		ThrowException_("execve" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::closeWr(int fd DEBUG_ARGS) {
	int ret;
	if ((ret = close(fd)) == -1) {
		ThrowException_("close" DEBUG_ARGS_NAMES);
	}
	return ret;
}

pid_t SyscallWrap::forkWr(const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = fork()) == -1) {
		(void) file;
		(void) func;
		(void) line;
		ThrowException_("fork" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::selectWr(int nfds,
						  fd_set *readfds,
						  fd_set *writefds,
						  fd_set *exceptfds,
						  struct timeval *timeout DEBUG_ARGS) {
	int ret;
	if ((ret = select(nfds, readfds, writefds, exceptfds, timeout)) == -1) {
		ThrowException_("select" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::socketWr(int domain,
						  int type,
						  int protocol DEBUG_ARGS) {
	int ret;
	if ((ret = socket(domain, type, protocol)) == -1) {
		ThrowException_("socket" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd, int cmd DEBUG_ARGS) {
	int ret;
	if ((ret = fcntl(fd, cmd)) == -1) {
		ThrowException_("fcntl" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 int64_t arg DEBUG_ARGS) {
	int ret;
	if ((ret = fcntl(fd, cmd, arg)) == -1) {
		ThrowException_("fcntl" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 struct flock *lock DEBUG_ARGS) {
	int ret;
	if ((ret = fcntl(fd, cmd, lock)) == -1) {
		ThrowException_("fcntl" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::acceptWr(int sockfd,
						  struct sockaddr *addr,
						  socklen_t *addrlen DEBUG_ARGS) {
	int ret;
	if ((ret = accept(sockfd, addr, addrlen)) == -1) {
		ThrowException_("accept" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::setsockoptWr(int sockfd,
							  int level,
							  int optname,
							  const void *optval,
							  socklen_t optlen DEBUG_ARGS) {
	int ret;
	if ((ret = setsockopt(sockfd, level, optname, optval, optlen)) == -1) {
		ThrowException_("setsockopt" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::bindWr(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen DEBUG_ARGS) {
	int ret;
	if ((ret = bind(sockfd, addr, addrlen)) == -1) {
		ThrowException_("bind" DEBUG_ARGS_NAMES);
	}
	return ret;
}

int SyscallWrap::listenWr(int sockfd,
						  int backlog DEBUG_ARGS) {
	int ret;
	if ((ret = listen(sockfd, backlog)) == -1) {
		ThrowException_("listen" DEBUG_ARGS_NAMES);
	}
	return ret;
}

void SyscallWrap::ThrowException_(const std::string &sys_call_name DEBUG_ARGS) {
	std::ostringstream ss;
	ss << sys_call_name << ": " << std::strerror(errno);
#ifdef DBG
	ss << ", in " << func << "(): " << file << ":" << line;
#endif
	throw std::runtime_error(ss.str());
}

