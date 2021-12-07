#include <ServerConfig.hpp>

ServerConfig::ServerConfig() :
	listen_address(INADDR_LOOPBACK),
	listen_port(8080),
	common(),
	default_server(false) {}

ServerConfig::ServerConfig(
	uint32_t listen_address,
	uint16_t listen_port,
	CommonConfig common,
	bool default_server) :
	listen_address(listen_address),
	listen_port(listen_port),
	common(common),
	default_server(default_server) {}
