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
# define dbgargs() , const std::string &file, const std::string &func, \
std::size_t line
# else
# define dbgargs()
#endif

class SyscallWrap {
	public:
		static int pipeWr(int pipefd[2] dbgargs());
		static int dupWr(int oldfd dbgargs());
		static int dup2Wr(int oldfd, int newfd dbgargs());
		static int execveWr(const char *pathname,
							char *const argv[],
							char *const envp[] dbgargs());
		static int closeWr(int fd dbgargs());
		static pid_t forkWr(const std::string &file,
							const std::string &func,
							std::size_t line);
		static int selectWr(int nfds,
							fd_set *readfds,
							fd_set *writefds,
							fd_set *exceptfds,
							struct timeval *timeout dbgargs());
		static int socketWr(int domain,
							int type,
							int protocol dbgargs());
		static int fcntlWr(int fd,
						   int cmd dbgargs());
		static int fcntlWr(int fd,
						   int cmd,
						   int64_t arg dbgargs());
		static int fcntlWr(int fd,
						   int cmd,
						   struct flock *lock dbgargs());
		static int acceptWr(int sockfd,
							struct sockaddr *addr,
							socklen_t *addrlen dbgargs());
		static int setsockoptWr(int sockfd,
								int level,
								int optname,
								const void *optval,
								socklen_t optlen dbgargs());
		static int bindWr(int sockfd,
						  const struct sockaddr *addr,
						  socklen_t addrlen dbgargs());
		static int listenWr(int sockfd,
							int backlog dbgargs());
#ifdef DBG

 private:
		static void AddDebuggingInfo_(const std::string &file,
									  const std::string &sys_call_name,
									  const std::string &func,
									  std::size_t line);
#endif
};

#endif  // SRCS_INCS_SYSCALLWRAP_HPP_
