#ifndef SRCS_INCS_COMMONCONFIG_HPP_
#define SRCS_INCS_COMMONCONFIG_HPP_
#include <stdint.h>
#include <map>
#include <string>
#include <vector>

struct CommonConfig {
	// typedefs
	typedef std::string									FileExtension;
	typedef	std::string									BinaryHandlerPath;
	typedef std::map<FileExtension, BinaryHandlerPath>	CgiAssignMap;
	typedef uint16_t									ErrorPageCode;
	typedef std::string									ErrorPageUri;
	typedef std::map<ErrorPageCode, ErrorPageUri>		ErrorPagesMap;

	// directives
	std::string		root;
	uint32_t		client_max_body_size;
	bool			autoindex;
	std::string		index;
	ErrorPagesMap	error_pages;
	std::string		upload_store;
	uint16_t		return_status;
	std::string		return_url;
	CgiAssignMap	cgi_assign;

	CommonConfig();
};

#endif  // SRCS_INCS_COMMONCONFIG_HPP_
