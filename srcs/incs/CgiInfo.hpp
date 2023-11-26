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
	CgiInfo(pid_t new_pid, int new_cgi_output_fd,
			const ErrorPagesMap &new_error_pages, const std::string &new_root_path);
	pid_t			pid;
	int				cgi_output_fd;
	ErrorPagesMap	error_pages;
	std::string		root_path;
};

#endif  // SRCS_INCS_CGIINFO_HPP_
