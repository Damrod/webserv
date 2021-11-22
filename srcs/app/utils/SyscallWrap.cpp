#include <SyscallWrap.hpp>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

int SyscallWrap::pipeWr(int pipefd[2]) {
	int ret;
	if ((ret = pipe(pipefd)) == -1) {
		throw std::runtime_error(std::strerror(errno));;
	}
	return ret;
}

int SyscallWrap::dupWr(int oldfd) {
	int ret;
	if ((ret = dup(oldfd)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int SyscallWrap::dup2Wr(int oldfd, int newfd) {
	int ret;
	if ((ret = dup2(oldfd, newfd)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int SyscallWrap::execveWr(const char *pathname, char *const argv[],
			 char *const envp[]) {
	int ret;
	if ((ret = execve(pathname, argv, envp)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int SyscallWrap::closeWr(int fd) {
	int ret;
	if ((ret = close(fd)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

pid_t SyscallWrap::forkWr(void) {
	int ret;
	if ((ret = fork()) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

int SyscallWrap::selectWr(int nfds, fd_set *readfds, fd_set *writefds,
					fd_set *exceptfds, struct timeval *timeout) {
	int ret;
	if ((ret = select(nfds, readfds, writefds, exceptfds, timeout)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::socketWr(int domain, int type, int protocol) {
	int ret;
	if ((ret = socket(domain, type, protocol)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd, int cmd) {
	int ret;
	if ((ret = fcntl(fd, cmd)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd, int cmd, int64_t arg) {
	int ret;
	if ((ret = fcntl(fd, cmd, arg)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::fcntlWr(int fd, int cmd, struct flock *lock) {
	int ret;
	if ((ret = fcntl(fd, cmd, lock)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::acceptWr(int sockfd, struct sockaddr *addr,
	socklen_t *addrlen) {
	int ret;
	if ((ret = accept(sockfd, addr, addrlen)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::setsockoptWr(int sockfd, int level, int optname,
					const void *optval, socklen_t optlen) {
	int ret;
	if ((ret = setsockopt(sockfd, level, optname, optval, optlen)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::bindWr(int sockfd, const struct sockaddr *addr,
			  socklen_t addrlen) {
	int ret;
	if ((ret = bind(sockfd, addr, addrlen)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

int SyscallWrap::listenWr(int sockfd, int backlog) {
	int ret;
	if ((ret = listen(sockfd, backlog)) == -1) {
		throw std::runtime_error(std::strerror(errno));
	}
	return ret;
}

//  ssize_t SyscallWrap::recvWr(int sockfd, void *buf, size_t len,
//  int flags);
//  ssize_t SyscallWrap::sendWr(int sockfd, const void *buf, size_t len,
//  int flags);

