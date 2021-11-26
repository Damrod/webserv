#ifndef SRCS_INCS_IRESPONSEFACTORY_HPP_
#define SRCS_INCS_IRESPONSEFACTORY_HPP_
#include <IResponse.hpp>

class IResponseFactory {
	public:
		virtual IResponse * Response() = 0;
		virtual ~IResponseFactory() {}
};

#endif  // SRCS_INCS_IRESPONSEFACTORY_HPP_
