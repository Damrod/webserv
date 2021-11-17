#ifndef SRCS_INCS_STRINGUTILS_HPP_
#define SRCS_INCS_STRINGUTILS_HPP_
#include <ios>
#include <sstream>
#include <string>
#include <cctype>

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
char		*DuplicateString(const std::string &str);
std::string	DecodeUrl(const std::string &encoded_url);

template <typename T>
std::string	ValueToString(const T &value) {
	std::stringstream out;
	out << value;
	return out.str();
}

#endif  // SRCS_INCS_STRINGUTILS_HPP_
