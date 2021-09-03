#ifndef SRCS_INCS_LOCATION_HPP_
#define SRCS_INCS_LOCATION_HPP_
#include <string>
#include <CommonConfig.hpp>

struct	Location {
	CommonConfig	common;
	/* More location settings */

	explicit Location(const CommonConfig &common);
};

#endif  // SRCS_INCS_LOCATION_HPP_
