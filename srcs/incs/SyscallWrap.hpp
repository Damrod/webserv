#ifndef SRCS_INCS_SYSCALLWRAP_HPP_
#define SRCS_INCS_SYSCALLWRAP_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <string>

class SyscallWrap {
	public:
		static int pipeWr(int pipefd[2],
						  const std::string &file,
						  const std::string &func,
						  std::size_t line);
		static int dupWr(int oldfd,
						 const std::string &file,
						 const std::string &func,
						 std::size_t line);
		static int dup2Wr(int oldfd,
						  int newfd,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line);
		static int execveWr(const char *pathname,
							char *const argv[],
							char *const envp[],
							const std::string &file,
							const std::string &func,
							std::size_t line);
		static int closeWr(int fd,
						   const std::string &file,
						   const std::string &func,
						   std::size_t line);
		static pid_t forkWr(const std::string &file,
							const std::string &func,
							std::size_t line);
		static int selectWr(int nfds,
							fd_set *readfds,
							fd_set *writefds,
							fd_set *exceptfds,
							struct timeval *timeout,
							const std::string &file,
							const std::string &func,
							std::size_t line);
		static int socketWr(int domain,
							int type,
							int protocol,
							const std::string &file,
							const std::string &func,
							std::size_t line);
		static int fcntlWr(int fd,
						   int cmd,
						   const std::string &file,
						   const std::string &func,
						   std::size_t line);
		static int fcntlWr(int fd,
						   int cmd,
						   int64_t arg,
						   const std::string &file,
						   const std::string &func,
						   std::size_t line);
		static int fcntlWr(int fd,
						   int cmd,
						   struct flock *lock,
						   const std::string &file,
						   const std::string &func,
						   std::size_t line);
		static int acceptWr(int sockfd,
							struct sockaddr *addr,
							socklen_t *addrlen,
							const std::string &file,
							const std::string &func,
							std::size_t line);
		static int setsockoptWr(int sockfd,
								int level,
								int optname,
								const void *optval,
								socklen_t optlen,
								const std::string &file,
								const std::string &func,
								std::size_t line);
		static int bindWr(int sockfd,
						  const struct sockaddr *addr,
						  socklen_t addrlen,
						  const std::string &file,
						  const std::string &func,
						  std::size_t line);
		static int listenWr(int sockfd,
							int backlog,
							const std::string &file,
							const std::string &func,
							std::size_t line);
//  static ssize_t recvWr(int sockfd, void *buf, size_t len, int flags);
//  static ssize_t sendWr(int sockfd, const void *buf, size_t len, int flags);
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
