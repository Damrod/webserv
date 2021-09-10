#ifndef SRCS_INCS_CONFIG_HPP_
#define SRCS_INCS_CONFIG_HPP_
#include <queue>
#include <string>
#include <ServerConfig.hpp>

class Config {
	private:
		std::queue<ServerConfig>	servers_settings_;

	public:
		bool	LoadFile(const std::string &pathname);
		std::queue<ServerConfig>	GetServersSettings() const;
};

#endif  // SRCS_INCS_CONFIG_HPP_
