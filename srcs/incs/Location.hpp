#ifndef SRCS_INCS_LOCATION_HPP_
#define SRCS_INCS_LOCATION_HPP_
#include <string>
#include <ServerSettings.hpp>

struct ServerSettings;

struct	Location {
	std::string	root;
	/* More server settings */

	explicit Location(const ServerSettings &servers_settings);
};

#endif  // SRCS_INCS_LOCATION_HPP_
