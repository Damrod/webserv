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
#include <HttpResponse.hpp>
#include <RequestLocation.hpp>
#include <SyscallWrap.hpp>
#include <Utils.hpp>

class CGI {
 public:
	CGI(const HttpRequest &request, const RequestLocation &location);
	virtual ~CGI(void);
	int ExecuteCGI(void);

 private:
	CGI(const CGI &);
	CGI & operator=(const CGI &);

	std::string GetExecutable_(const std::string &extension);
	std::map<std::string, std::string> MakeEnv_(void);
	char **MakeCEnv_(void);
	void CloseAssign_(int *fd);
	const HttpRequest &request_;
	const RequestLocation *request_location_;
	const std::string arg_path_;
	const std::string exec_path_;
	const std::map<std::string, std::string> CGIenvMap_;
	char * const *CGIenv_;
	int fds_[2];
};

#endif  // SRCS_INCS_CGI_HPP_
