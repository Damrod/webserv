#ifndef SRCS_INCS_SERVERCONFIG_HPP_
#define SRCS_INCS_SERVERCONFIG_HPP_
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <CommonConfig.hpp>
#include <Location.hpp>

struct ServerConfig {
	uint32_t					listen_address;
	uint16_t					listen_port;
	std::vector<std::string>	server_name;
	CommonConfig				common;
	bool						default_server;

	// A server can have multiple locations
	std::vector<Location>		locations;

	ServerConfig();
	ServerConfig(
		uint32_t listen_address,
		uint16_t listen_port,
		CommonConfig common,
		bool default_server);
};

#endif  // SRCS_INCS_SERVERCONFIG_HPP_
