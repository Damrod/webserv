#ifndef SRCS_INCS_SYSCALLWRAP_HPP_
#define SRCS_INCS_SYSCALLWRAP_HPP_
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <string>

#ifdef DBG
# define DEBUG_INFO , __FILE__, __FUNCTION__, __LINE__
# define DEBUG_ARGS , const std::string &file, const std::string &func, \
std::size_t line
# else
# define DEBUG_INFO
# define DEBUG_ARGS
#endif

class SyscallWrap {
	public:
		static int pipeWr(int pipefd[2] DEBUG_ARGS);
		static int dupWr(int oldfd DEBUG_ARGS);
		static int dup2Wr(int oldfd, int newfd DEBUG_ARGS);
		static int execveWr(const char *pathname,
							char *const argv[],
							char *const envp[] DEBUG_ARGS);
		static int closeWr(int fd DEBUG_ARGS);
		static pid_t forkWr(const std::string &file,
							const std::string &func,
							std::size_t line);
		static int selectWr(int nfds,
							fd_set *readfds,
							fd_set *writefds,
							fd_set *exceptfds,
							struct timeval *timeout DEBUG_ARGS);
		static int socketWr(int domain,
							int type,
							int protocol DEBUG_ARGS);
		static int fcntlWr(int fd,
						   int cmd DEBUG_ARGS);
		static int fcntlWr(int fd,
						   int cmd,
						   int64_t arg DEBUG_ARGS);
		static int fcntlWr(int fd,
						   int cmd,
						   struct flock *lock DEBUG_ARGS);
		static int acceptWr(int sockfd,
							struct sockaddr *addr,
							socklen_t *addrlen DEBUG_ARGS);
		static int setsockoptWr(int sockfd,
								int level,
								int optname,
								const void *optval,
								socklen_t optlen DEBUG_ARGS);
		static int bindWr(int sockfd,
						  const struct sockaddr *addr,
						  socklen_t addrlen DEBUG_ARGS);
		static int listenWr(int sockfd,
							int backlog DEBUG_ARGS);
#ifdef DBG

 private:
		static void AddDebuggingInfo_(const std::string &file,
									  const std::string &sys_call_name,
									  const std::string &func,
									  std::size_t line);
#endif
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
