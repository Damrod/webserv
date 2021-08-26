#ifndef SRCS_INCS_SERVERSETTINGS_HPP_
#define SRCS_INCS_SERVERSETTINGS_HPP_
#include <stdint.h>
#include <string>
#include <vector>
#include <Location.hpp>

struct Location;

struct ServerSettings {
	uint32_t					listen_address;
	uint16_t					listen_port;
	std::vector<std::string>	server_name;
	std::string					root;
	/* More server settings */

	// A server can have multiple locations
	std::vector<Location>		locations;

	ServerSettings();
};

#endif  // SRCS_INCS_SERVERSETTINGS_HPP_
