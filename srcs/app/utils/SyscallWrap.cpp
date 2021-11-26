#include <SyscallWrap.hpp>

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
