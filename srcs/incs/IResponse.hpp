#ifndef SRCS_INCS_IRESPONSE_HPP_
#define SRCS_INCS_IRESPONSE_HPP_

#include <string>

class IResponse {
	public:
		virtual	std::string Content() = 0;
		virtual bool	KeepAlive() = 0;
};

#endif  // SRCS_INCS_IRESPONSE_HPP_
