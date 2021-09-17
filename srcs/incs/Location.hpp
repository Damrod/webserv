#ifndef SRCS_INCS_LOCATION_HPP_
#define SRCS_INCS_LOCATION_HPP_
#include <string>
#include <vector>
#include <CommonConfig.hpp>

struct	Location {
	CommonConfig			common;
	typedef std::string		HttpMethod;
	std::vector<HttpMethod>	limit_except;

	explicit Location(const CommonConfig &common);
};

#endif  // SRCS_INCS_LOCATION_HPP_
