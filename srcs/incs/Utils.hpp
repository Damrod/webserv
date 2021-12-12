#ifndef SRCS_INCS_UTILS_HPP_
#define SRCS_INCS_UTILS_HPP_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <sstream>
#include <string>
#include <MimeTypes.hpp>

std::string	TrimString(const std::string &str, const std::string &trim_chars);
std::string	ToLowerString(std::string str);
std::string	ToUpperString(std::string str);
char		*DuplicateString(const std::string &str);
std::string	DecodeUrl(const std::string &encoded_url);
std::string	PathExtension(const std::string &path);

template <typename T>
std::string	ValueToString(const T &value) {
	std::stringstream out;
	out << value;
	return out.str();
}

#endif  // SRCS_INCS_UTILS_HPP_
