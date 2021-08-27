#ifndef SRCS_INCS_COMMONSETTINGS_HPP_
#define SRCS_INCS_COMMONSETTINGS_HPP_
#include <stdint.h>
#include <string>

struct CommonSettings {
	std::string	root;
	uint32_t	client_max_body_size;
	bool		autoindex;
	std::string	index;

	CommonSettings();
};

#endif  // SRCS_INCS_COMMONSETTINGS_HPP_
