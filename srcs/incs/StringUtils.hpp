#ifndef SRCS_INCS_STRINGUTILS_HPP_
#define SRCS_INCS_STRINGUTILS_HPP_
#include <stdint.h>
#include <string>

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
std::string UInt16ToStr(uint16_t nbr);

#endif  // SRCS_INCS_STRINGUTILS_HPP_
