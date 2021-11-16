#ifndef SRCS_INCS_CGI_HPP_
#define SRCS_INCS_CGI_HPP_

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <HttpRequest.hpp>
#include <RequestConfig.hpp>
#include <StringUtils.hpp>
#include <SyscallWrap.hpp>

class CGI {
 public:
	CGI(const HttpRequest &request, const RequestConfig &location,
		const std::string &extension);
	virtual ~CGI(void);
	void ExecuteCGI(void);
	int  GetExecReturn() const;
	std::string  GetRawOutput() const;
	std::string  GetBody() const;
	std::map<std::string, std::string>	GetHeaders() const;

 private:
	enum {
		BUFFER_SIZE = 512,
	};
	std::string GetExecutable_(const std::string &extension);
	std::map<std::string, std::string> MakeEnv_(void);
	char **MakeCEnv_(void);
	void ParseCGIOut_(void);
	void SetHeaders_(void);
	size_t NextStatementThrowing_(const std::string &str,
								  const std::string &separator,
								  bool needsSeparator,
								  size_t line);
	void WriteAll_(int fd, const void *buf, size_t count);
	void CloseAssign_(int *fd);
	int execRet_;
	std::string CGIout_;
	std::string CGIoutBody_;
	std::string CGIoutHeaders_;
	std::map<std::string, std::string> parsedHeaders_;
	const HttpRequest &request_;
	const RequestConfig *requestConfig_;
	const std::string reqBody_;
	const std::string arg_path_;
	const std::string exec_path_;
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
