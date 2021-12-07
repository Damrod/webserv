#ifndef SRCS_INCS_CGIINFO_HPP_
#define SRCS_INCS_CGIINFO_HPP_
#include <sys/types.h>
#include <stdint.h>
#include <map>
#include <string>

struct CgiInfo {
	typedef uint16_t									ErrorPageCode;
	typedef std::string									ErrorPageUri;
	typedef std::map<ErrorPageCode, ErrorPageUri>		ErrorPagesMap;

	CgiInfo();
	CgiInfo(const pid_t &, const int &, const ErrorPagesMap &);
	pid_t			pid;
	int				cgi_output_fd;
	ErrorPagesMap	error_pages;
};

#endif  // SRCS_INCS_CGIINFO_HPP_
