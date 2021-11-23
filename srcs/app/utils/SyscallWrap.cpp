#include <SyscallWrap.hpp>

int SyscallWrap::pipeWr(int pipefd[2],
						const std::string &file,
						const std::string &func,
						std::size_t line) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::dupWr(int oldfd,
					   const std::string &file,
					   const std::string &func,
					   std::size_t line) {
	int ret;
	if ((ret = dup(oldfd)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::dup2Wr(int oldfd,
						int newfd,
						const std::string &file,
						const std::string &func,
						std::size_t line) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1) {		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::execveWr(const char *pathname,
						  char *const argv[],
						  char *const envp[],
						  const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = execve(pathname, argv, envp)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::closeWr(int fd,
						 const std::string &file,
						 const std::string &func,
						 std::size_t line) {
	int ret;
	if ((ret = close(fd)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

pid_t SyscallWrap::forkWr(const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = fork()) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::selectWr(int nfds,
						  fd_set *readfds,
						  fd_set *writefds,
						  fd_set *exceptfds,
						  struct timeval *timeout,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = select(nfds, readfds, writefds, exceptfds, timeout)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::socketWr(int domain,
						  int type,
						  int protocol,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = socket(domain, type, protocol)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 const std::string &file,
						 const std::string &func,
						 std::size_t line) {
	int ret;
	if ((ret = fcntl(fd, cmd)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 int64_t arg,
						 const std::string &file,
						 const std::string &func,
						 std::size_t line) {
	int ret;
	if ((ret = fcntl(fd, cmd, arg)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd,
						 int cmd,
						 struct flock *lock,
						 const std::string &file,
						 const std::string &func,
						 std::size_t line) {
	int ret;
	if ((ret = fcntl(fd, cmd, lock)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::acceptWr(int sockfd,
						  struct sockaddr *addr,
						  socklen_t *addrlen,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = accept(sockfd, addr, addrlen)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::setsockoptWr(int sockfd,
							  int level,
							  int optname,
							  const void *optval,
							  socklen_t optlen,
							  const std::string &file,
							  const std::string &func,
							  std::size_t line) {
	int ret;
	if ((ret = setsockopt(sockfd, level, optname, optval, optlen)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::bindWr(int sockfd,
						const struct sockaddr *addr,
						socklen_t addrlen,
						const std::string &file,
						const std::string &func,
						std::size_t line) {
	int ret;
	if ((ret = bind(sockfd, addr, addrlen)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

int SyscallWrap::listenWr(int sockfd,
						  int backlog,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line) {
	int ret;
	if ((ret = listen(sockfd, backlog)) == -1) {
		std::ostringstream ss;
		ss << func << ": " << __FUNCTION__ << ": " << std::strerror(errno)
		<< ", in " << file << ":"<< line;
		throw std::runtime_error(ss.str());;
	}
	return ret;
}

//  ssize_t SyscallWrap::recvWr(int sockfd, void *buf, size_t len,
//  int flags);
//  ssize_t SyscallWrap::sendWr(int sockfd, const void *buf, size_t len,
//  int flags);

