#ifndef SRCS_INCS_COMMONDEFINITIONS_HPP_
#define SRCS_INCS_COMMONDEFINITIONS_HPP_

#include <stdint.h>
#include <string>

namespace Constants {

bool IsValidMethod(const std::string &method);
bool IsReturnStatusRedirection(int16_t status);

const char kValidHttpMethods[8][8] = {"GET", "HEAD", "POST",
		"PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};

const uint16_t kRedirectionReturnStatus[5] = {301,
		302, 303, 307, 308};

}  // namespace Constants

#endif  // SRCS_INCS_COMMONDEFINITIONS_HPP_
