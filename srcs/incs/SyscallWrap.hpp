#ifndef SRCS_INCS_SYSCALLWRAP_HPP_
#define SRCS_INCS_SYSCALLWRAP_HPP_
#include <sys/types.h>
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

class SyscallWrap {
	public:
		static int pipeWr(int pipefd[2]);
		static int dupWr(int oldfd);
		static int dup2Wr(int oldfd, int newfd);
		static int execveWr(const char *pathname, char *const argv[],
						   char *const envp[]);
		static int closeWr(int fd);
		static pid_t forkWr(void);
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
