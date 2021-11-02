#ifndef SRCS_INCS_HTTPDELETERESPONSE_HPP_
#define SRCS_INCS_HTTPDELETERESPONSE_HPP_

#include <string>
#include "IResponse.hpp"

class HttpDeleteResponse: public IResponse {
	public:
		std::string content();
};

#endif //SRCS_INCS_HTTPDELETERESPONSE_HPP_
