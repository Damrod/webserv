#include <CgiInfo.hpp>

CgiInfo::CgiInfo() : pid(-1), cgi_output_fd(-1) {}

CgiInfo::CgiInfo(const pid_t &new_pid,
		const int &new_cgi_output_fd,
		const ErrorPagesMap &new_error_pages,
		const std::string &new_root_path)
	: pid(new_pid),
	cgi_output_fd(new_cgi_output_fd),
	error_pages(new_error_pages),
	root_path(new_root_path) {}
