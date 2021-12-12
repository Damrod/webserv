#ifndef SRCS_INCS_CGI_HPP_
#define SRCS_INCS_CGI_HPP_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
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
#include <HttpHeaders.hpp>
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

	typedef std::map<std::string, std::string> EnvsMap_;
	std::string GetExecutable_();
	std::map<std::string, std::string> MakeEnv_(void);
	void AddHttpEnv_(EnvsMap_ *envs);
	char **MakeCEnv_(void);
	void CloseAssign_(int *fd);
	std::string MakeClientAddress_();
	std::string GetClientAddress_() const;
	const HttpRequest &request_;
	const RequestConfig *requestConfig_;
	const File &file_;
	std::string client_address_;
	const std::string arg_path_;
	const std::string exec_path_;
	const EnvsMap_ CGIenvMap_;
	char * const *CGIenv_;
	int fds_[2];
};

#endif  // SRCS_INCS_CGI_HPP_
