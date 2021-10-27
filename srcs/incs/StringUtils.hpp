#ifndef SRCS_INCS_STRINGUTILS_HPP_
#define SRCS_INCS_STRINGUTILS_HPP_
#include <string>

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
std::string	UnsignedToStr(std::size_t nbr);

#endif  // SRCS_INCS_STRINGUTILS_HPP_
