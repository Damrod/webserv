#ifndef SRCS_INCS_REQUESTCONFIG_HPP_
#define SRCS_INCS_REQUESTCONFIG_HPP_
#include <string>
#include <vector>
#include <ServerConfig.hpp>

// All config related to a request in a Sever

class RequestConfig {
	private:
		const Location					*location_;
		RequestConfig();
		RequestConfig(const RequestConfig &);
		RequestConfig &	operator=(const RequestConfig &);

	public:
		RequestConfig(const ServerConfig &server_config,
						const std::string &request_path);
		const CommonConfig				&common;
		const std::string				*path;
		const std::vector<std::string>	*limit_except;
		bool							HasLocation() const;
		const Location					*FindLocation_(
											const ServerConfig &server_config,
											const std::string &request_path)
											const;
};

#endif  // SRCS_INCS_REQUESTCONFIG_HPP_
