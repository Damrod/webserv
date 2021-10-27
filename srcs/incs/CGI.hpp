#ifndef SRCS_INCS_CGI_HPP_
#define SRCS_INCS_CGI_HPP_

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <RequestLocation.hpp>

class CGI {
 public:
	CGI(const HttpRequest &request, const RequestLocation &location,
		const std::string &extension, HttpResponse *response);
	virtual ~CGI(void);
	void ExecuteCGI(void);
	int  GetExecReturn() const;

 private:
	enum {
		BUFFER_SIZE = 512,
	};
	class SyscallWrap {
	public:
		static ssize_t writeWr(int fd, const void *buf, size_t count);
		static ssize_t readWr(int fd, void *buf, size_t count);
		static int pipeWr(int pipefd[2]);
		static int dup2Wr(int oldfd, int newfd);
		static int execveWr(const char *pathname, char *const argv[],
						   char *const envp[]);
		static int closeWr(int *fd);
		static pid_t forkWr(void);
		static pid_t waitpidWr(pid_t pid, int *wstatus, int options);
	};
	std::string GetExecutable_(const std::string &extension);
	std::map<std::string, std::string> MakeEnv_(void);
	char **MakeCEnv_(void);
	void ParseCGIOut_(void);
	char *StrDupWrapper_(const std::string &str);
	void SetHeaders_(void);
	size_t NextStatementThrowing_(const std::string &str,
								  const std::string &separator,
								  bool needsSeparator,
								  size_t line);
	void WriteAll_(int fd, const void *buf, size_t count);
	int execRet_;
	std::string CGIout_;
	std::string CGIoutHeaders_;
	std::string CGIoutBody_;
	const HttpRequest &request_;
	const RequestLocation *request_location_;
	const std::string reqBody_;
	const std::string arg_path_;
	const std::string exec_path_;
	HttpResponse *response_;
	std::string raw_response_;
	const std::map<std::string, std::string> CGIenvMap_;
	char * const *CGIenv_;
	int pipes_[2];
	int pipes2_[2];
	static const char kCRLF_[3];
};

//  SyscallsErrorThrowing
//  template <typename T>
//  toString(const T &data);

#endif  // SRCS_INCS_CGI_HPP_
