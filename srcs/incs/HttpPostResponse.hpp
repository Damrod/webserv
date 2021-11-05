#ifndef SRCS_INCS_HTTPPOSTRESPONSE_HPP_
#define SRCS_INCS_HTTPPOSTRESPONSE_HPP_

#include <string>
#include "IResponse.hpp"

class HttpPostResponse: public IResponse {
	public:
		std::string content();
};

#endif  // SRCS_INCS_HTTPPOSTRESPONSE_HPP_
