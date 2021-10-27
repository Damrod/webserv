#ifndef SRCS_INCS_STRINGUTILS_HPP_
#define SRCS_INCS_STRINGUTILS_HPP_
#include <sstream>
#include <string>

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
char		*DuplicateString(const std::string &str);

template <typename T>
std::string	ValueToString(const T &value) {
	std::stringstream out;
	out << value;
	return out.str();
}

#endif  // SRCS_INCS_STRINGUTILS_HPP_
