#ifndef SRCS_INCS_IRESPONSE_HPP_
#define SRCS_INCS_IRESPONSE_HPP_
#include <string>

class IResponse {
	public:
		virtual	std::string content() = 0;
};

#endif //SRCS_INCS_ARESPONSE_HPP_
