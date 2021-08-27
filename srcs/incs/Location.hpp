#ifndef SRCS_INCS_LOCATION_HPP_
#define SRCS_INCS_LOCATION_HPP_
#include <string>
#include <CommonSettings.hpp>

struct	Location {
	CommonSettings	common;
	/* More location settings */

	explicit Location(const CommonSettings &common);
};

#endif  // SRCS_INCS_LOCATION_HPP_
