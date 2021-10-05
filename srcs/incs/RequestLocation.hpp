#ifndef SRCS_INCS_REQUESTLOCATION_HPP_
#define SRCS_INCS_REQUESTLOCATION_HPP_
#include <string>
#include <vector>
#include <ServerConfig.hpp>

class RequestLocation {
	private:
		const Location					*location_;
		RequestLocation();
		RequestLocation(const RequestLocation &);
		RequestLocation &	operator=(const RequestLocation &);

	public:
		RequestLocation(const ServerConfig &server_config,
						const std::string &request_path);
		const CommonConfig				&common;
		const std::vector<std::string>	*limit_except;
		bool							HasLocation() const;
		const Location					*FindLocation_(
											const ServerConfig &server_config,
											const std::string &request_path)
											const;
};

#endif  // SRCS_INCS_REQUESTLOCATION_HPP_
