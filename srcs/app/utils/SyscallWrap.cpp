#include <SyscallWrap.hpp>

int SyscallWrap::pipeWr(int pipefd[2] dbgargs()) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "pipe", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::dupWr(int oldfd dbgargs()) {
	int ret;
	if ((ret = dup(oldfd)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "dup", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::dup2Wr(int oldfd,
						int newfd dbgargs()) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "dup2", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::execveWr(const char *pathname,
						  char *const argv[],
						  char *const envp[] dbgargs()) {
	int ret;
	if ((ret = execve(pathname, argv, envp)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "execve", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::closeWr(int fd dbgargs()) {
	int ret;
	if ((ret = close(fd)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "close", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

pid_t SyscallWrap::forkWr(const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = fork()) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "fork", func, line);
#else
		(void) file;
		(void) func;
		(void) line;
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::selectWr(int nfds,
						  fd_set *readfds,
						  fd_set *writefds,
						  fd_set *exceptfds,
						  struct timeval *timeout dbgargs()) {
	int ret;
	if ((ret = select(nfds, readfds, writefds, exceptfds, timeout)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "select", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::socketWr(int domain,
						  int type,
						  int protocol dbgargs()) {
	int ret;
	if ((ret = socket(domain, type, protocol)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "socket", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd, int cmd dbgargs()) {
	int ret;
	if ((ret = fcntl(fd, cmd)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "fcntl", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 int64_t arg dbgargs()) {
	int ret;
	if ((ret = fcntl(fd, cmd, arg)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "fcntl", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 struct flock *lock dbgargs()) {
	int ret;
	if ((ret = fcntl(fd, cmd, lock)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "fcntl", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::acceptWr(int sockfd,
						  struct sockaddr *addr,
						  socklen_t *addrlen dbgargs()) {
	int ret;
	if ((ret = accept(sockfd, addr, addrlen)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "accept", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::setsockoptWr(int sockfd,
							  int level,
							  int optname,
							  const void *optval,
							  socklen_t optlen dbgargs()) {
	int ret;
	if ((ret = setsockopt(sockfd, level, optname, optval, optlen)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "setsockopt", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::bindWr(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen dbgargs()) {
	int ret;
	if ((ret = bind(sockfd, addr, addrlen)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "bind", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

int SyscallWrap::listenWr(int sockfd,
						  int backlog dbgargs()) {
	int ret;
	if ((ret = listen(sockfd, backlog)) == -1) {
#ifdef DBG
		AddDebuggingInfo_(file, "listen", func, line);
#else
		throw std::runtime_error(std::strerror(errno));
#endif
	}
	return ret;
}

#ifdef DBG
void SyscallWrap::AddDebuggingInfo_(const std::string &file,
							  const std::string &sys_call_name,
							  const std::string &func,
							  std::size_t line) {
	std::ostringstream ss;
	ss << func << ": " << sys_call_name << ": " << std::strerror(errno)
	   << ", in " << file << ":"<< line;
	throw std::runtime_error(ss.str());
}
#endif

