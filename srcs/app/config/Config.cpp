#include <Config.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		ServerConfig	settings;
		servers_settings_.push(settings);
		return true;
	}
	return false;
}

std::queue<ServerConfig>	Config::GetServersSettings() const {
	return servers_settings_;
}
