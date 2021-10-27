#ifndef SRCS_INCS_SYSCALLWRAP_HPP_
#define SRCS_INCS_SYSCALLWRAP_HPP_
#include <sys/types.h>

class SyscallWrap {
	public:
		static ssize_t writeWr(int fd, const void *buf, size_t count);
		static ssize_t readWr(int fd, void *buf, size_t count);
		static int pipeWr(int pipefd[2]);
		static int dup2Wr(int oldfd, int newfd);
		static int execveWr(const char *pathname, char *const argv[],
						   char *const envp[]);
		static int closeWr(int fd);
		static pid_t forkWr(void);
		static pid_t waitpidWr(pid_t pid, int *wstatus, int options);
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
