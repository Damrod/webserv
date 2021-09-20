#ifndef SRCS_INCS_LOCATION_HPP_
#define SRCS_INCS_LOCATION_HPP_
#include <string>
#include <vector>
#include <CommonConfig.hpp>

struct	Location {
	std::string				path;
	CommonConfig			common;
	typedef std::string		HttpMethod;
	std::vector<HttpMethod>	limit_except;

	Location(const std::string &path, const CommonConfig &common);
};

#endif  // SRCS_INCS_LOCATION_HPP_
