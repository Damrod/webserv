#include <ServerConfig.hpp>

ServerConfig::ServerConfig()
	: listen_address(INADDR_LOOPBACK), listen_port(8080), common() {}

ServerConfig::ServerConfig(
	uint32_t listen_address,
	uint16_t listen_port,
	CommonConfig common) :
	listen_address(listen_address),
	listen_port(listen_port),
	common(common) {}
