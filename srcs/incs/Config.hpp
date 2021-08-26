#ifndef SRCS_INCS_CONFIG_HPP_
#define SRCS_INCS_CONFIG_HPP_
#include <queue>
#include <string>
#include <ServerSettings.hpp>

class Config {
	private:
		std::queue<ServerSettings>	servers_settings_;

	public:
		bool	LoadFile(const std::string &pathname);
		std::queue<ServerSettings>	GetServersSettings() const;
};

#endif  // SRCS_INCS_CONFIG_HPP_
