#ifndef SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
#define SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_

#include <string>
#include "IResponse.hpp"

class HttpRedirectionResponse: public IResponse {
	public:
		std::string content();
};

#endif //SRCS_INCS_HTTPREDIRECTIONRESPONSE_HPP_
