#ifndef SRCS_INCS_HTTPGETRESPONSE_HPP_
#define SRCS_INCS_HTTPGETRESPONSE_HPP_

#include <string>
#include "IResponse.hpp"

class HttpGetResponse: public IResponse {
	public:
		std::string content();
};

#endif  // SRCS_INCS_HTTPGETRESPONSE_HPP_
