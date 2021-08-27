#ifndef SRCS_INCS_SERVERSETTINGS_HPP_
#define SRCS_INCS_SERVERSETTINGS_HPP_
#include <stdint.h>
#include <string>
#include <vector>
#include <CommonSettings.hpp>
#include <Location.hpp>

struct ServerSettings {
	uint32_t					listen_address;
	uint16_t					listen_port;
	std::vector<std::string>	server_name;
	CommonSettings				common;

	// A server can have multiple locations
	std::vector<Location>		locations;

	ServerSettings();
};

#endif  // SRCS_INCS_SERVERSETTINGS_HPP_
