#ifndef SRCS_INCS_COMMONDEFINITIONS_HPP_
#define SRCS_INCS_COMMONDEFINITIONS_HPP_

#include <stdint.h>
#include <string>

namespace Constants {

bool IsValidMethod(const std::string &method);
bool IsReturnStatusRedirection(int16_t status);

extern const char kValidHttpMethods[8][8];
extern const uint16_t kRedirectionReturnStatus[5];

}

#endif  // SRCS_INCS_COMMONDEFINITIONS_HPP_
