#ifndef SRCS_INCS_CGI_HPP_
#define SRCS_INCS_CGI_HPP_

#include <unistd.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <SyscallWrap.hpp>
#include <Utils.hpp>
#include <File.hpp>
#include <CgiInfo.hpp>

class CGI {
 public:
	CGI(const HttpRequest &request, const RequestConfig &location,
		const File &file);
	virtual ~CGI(void);
	CgiInfo ExecuteCGI(void);

 private:
	CGI();
	CGI(const CGI &);
	CGI & operator=(const CGI &);

	std::string GetExecutable_();
	std::map<std::string, std::string> MakeEnv_(void);
	char **MakeCEnv_(void);
	void CloseAssign_(int *fd);
	const HttpRequest &request_;
	const RequestConfig *requestConfig_;
	const File &file_;
	const std::string arg_path_;
	const std::string exec_path_;
	const std::map<std::string, std::string> CGIenvMap_;
	char * const *CGIenv_;
	int fds_[2];
};

#endif  // SRCS_INCS_CGI_HPP_
