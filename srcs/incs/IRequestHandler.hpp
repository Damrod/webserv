#ifndef SRCS_INCS_IREQUESTHANDLER_HPP_
#define SRCS_INCS_IREQUESTHANDLER_HPP_
#include <string>

class IRequestHandler {
	public:
		virtual std::string	GetRawResponse() const = 0;
		virtual bool		GetKeepAlive() const = 0;
		virtual				~IRequestHandler() {}
};

#endif  // SRCS_INCS_IREQUESTHANDLER_HPP_
