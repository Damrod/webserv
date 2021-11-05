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

ssize_t SyscallWrap::writeWr(int fd, const void *buf, size_t count) {
	ssize_t ret;
	if ((ret = write(fd, buf, count)) == -1)
		throw std::runtime_error("Problem writing");
	return ret;
}

ssize_t SyscallWrap::readWr(int fd, void *buf, size_t count) {
	ssize_t ret;
	if ((ret = read(fd, buf, count)) == -1)
		throw std::runtime_error("Problem reading");
	return ret;
}

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

pid_t SyscallWrap::waitpidWr(pid_t pid, int *wstatus, int options) {
	pid_t ret;
	if ((ret = waitpid(pid, wstatus, options)) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}

pid_t SyscallWrap::forkWr(void) {
	int ret;
	if ((ret = fork()) == -1)
		throw std::runtime_error(std::strerror(errno));
	return ret;
}
