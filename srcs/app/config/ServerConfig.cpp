#include <ServerConfig.hpp>

ServerConfig::ServerConfig() :
	listen_address(INADDR_LOOPBACK),
	listen_port(8080),
	common(),
	default_server(false) {}
