#ifndef SRCS_INCS_COMMONCONFIG_HPP_
#define SRCS_INCS_COMMONCONFIG_HPP_
#include <stdint.h>
#include <string>

struct CommonConfig {
	std::string	root;
	uint32_t	client_max_body_size;
	bool		autoindex;
	std::string	index;

	CommonConfig();
};

#endif  // SRCS_INCS_COMMONCONFIG_HPP_
