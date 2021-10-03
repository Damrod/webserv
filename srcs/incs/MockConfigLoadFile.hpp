#ifndef SRCS_INCS_MOCKCONFIGLOADFILE_HPP_
#define SRCS_INCS_MOCKCONFIGLOADFILE_HPP_
#include <string>
#include <vector>
#include <ServerConfig.hpp>

class MockConfigLoadFile {
	public:
		MockConfigLoadFile();
		std::vector<ServerConfig>	GetServersConfigs() const;

	private:
		void	InitConfig_();
		std::vector<ServerConfig> servers_configs_;
};

#endif  // SRCS_INCS_MOCKCONFIGLOADFILE_HPP_
