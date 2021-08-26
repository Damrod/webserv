#include <ServerSettings.hpp>
#include <netinet/in.h>

ServerSettings::ServerSettings()
	: listen_address(INADDR_LOOPBACK), listen_port(8080), root("html") {}
