#include <Config.hpp>
#include <MockConfigLoadFile.hpp>

bool	Config::LoadFile(const std::string &pathname) {
	// Read and parse the configuration file and save to servers_settings_
	if (!pathname.empty()) {
		MockConfigLoadFile	mock_load_file;
		servers_settings_ = mock_load_file.GetServersConfigs();
		return true;
	}
	return false;
}

std::vector<ServerConfig>	Config::GetServersSettings() const {
	return servers_settings_;
}
