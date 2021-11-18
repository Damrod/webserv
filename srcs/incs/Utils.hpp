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
char		*DuplicateString(const std::string &str);
std::string	DecodeUrl(const std::string &encoded_url);
bool		IsExecutable(const std::string &path);
bool		IsValidPath(const std::string &path);
bool		IsDirectory(const std::string &path);
bool		IsRegularFile(const std::string &path);
std::string	PathExtension(const std::string &path);
std::string	GetMimeType(const std::string &file_path);
std::string	CurrentDate();

template <typename T>
std::string	ValueToString(const T &value) {
	std::stringstream out;
	out << value;
	return out.str();
}

#endif  // SRCS_INCS_UTILS_HPP_
