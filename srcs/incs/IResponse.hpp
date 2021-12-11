#ifndef SRCS_INCS_IRESPONSE_HPP_
#define SRCS_INCS_IRESPONSE_HPP_

#include <string>
#include <CgiInfo.hpp>

class IResponse {
	public:
		virtual	std::string Content() const = 0;
		virtual bool	KeepAlive() const = 0;
		virtual bool	IsCgi() const = 0;
		virtual CgiInfo	GetCgiInfo() const = 0;
		virtual ~IResponse() {}
};

#endif  // SRCS_INCS_IRESPONSE_HPP_
