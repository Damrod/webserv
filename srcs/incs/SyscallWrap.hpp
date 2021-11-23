#ifndef SRCS_INCS_SYSCALLWRAP_HPP_
#define SRCS_INCS_SYSCALLWRAP_HPP_
#include <sys/types.h>
#include <sys/socket.h>

class SyscallWrap {
	public:
		static int pipeWr(int pipefd[2]);
		static int dupWr(int oldfd);
		static int dup2Wr(int oldfd, int newfd);
		static int execveWr(const char *pathname, char *const argv[],
						   char *const envp[]);
		static int closeWr(int fd);
		static pid_t forkWr(void);
		static int selectWr(int nfds, fd_set *readfds, fd_set *writefds,
							  fd_set *exceptfds, struct timeval *timeout);
		static int socketWr(int domain, int type, int protocol);
		static int fcntlWr(int fd, int cmd);
		static int fcntlWr(int fd, int cmd, int64_t arg);
		static int fcntlWr(int fd, int cmd, struct flock *lock);
		static int acceptWr(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
		static int setsockoptWr(int sockfd, int level, int optname,
							  const void *optval, socklen_t optlen);
		static int bindWr(int sockfd, const struct sockaddr *addr,
					socklen_t addrlen);
		static int listenWr(int sockfd, int backlog);
//  static ssize_t recvWr(int sockfd, void *buf, size_t len, int flags);
//  static ssize_t sendWr(int sockfd, const void *buf, size_t len, int flags);
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
