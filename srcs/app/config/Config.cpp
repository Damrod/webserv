#include <Config.hpp>
#include <parser/ParserManager.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		ParserManager parser("srcs/config_files/AGVTest.conf");
		servers_settings_ = parser.GetServersSettings();;
		return true;
	}
	return false;
}

std::vector<ServerConfig>	Config::GetServersSettings() const {
	return servers_settings_;
}
