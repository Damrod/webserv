#ifndef SRCS_INCS_IREQUESTHANDLER_HPP_
#define SRCS_INCS_IREQUESTHANDLER_HPP_
#include <string>
#include <IRequest.hpp>

class IRequestHandler {
	public:
		virtual bool		GetKeepAlive() const = 0;
		virtual std::string	BuildResponse(IRequest *) = 0;
		virtual				~IRequestHandler() {}
};

#endif  // SRCS_INCS_IREQUESTHANDLER_HPP_
