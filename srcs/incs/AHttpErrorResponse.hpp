#ifndef SRCS_INCS_AHTTPERRORRESPONSE_HPP_
#define SRCS_INCS_AHTTPERRORRESPONSE_HPP_

#include <AHttpResponse.hpp>

class AHttpErrorResponse: public AHttpResponse {
	public:
		explicit	AHttpErrorResponse(
						std::size_t status_code);
					~AHttpErrorResponse();
		std::string	RawContent() const;

};

std::ostream&	operator<<(std::ostream &os, const AHttpErrorResponse &response);

#endif  // SRCS_INCS_AHTTPERRORRESPONSE_HPP_
