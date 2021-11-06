#include <Utils.hpp>

std::string	TrimString(const std::string &str, const std::string &trim_chars) {
	const std::size_t	start_position = str.find_first_not_of(trim_chars);

	if (start_position == std::string::npos)
		return "";
	const std::size_t	end_position = str.find_last_not_of(trim_chars) + 1;
	return str.substr(start_position, end_position - start_position);
}

std::string	ToLowerString(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

char *DuplicateString(const std::string &str) {
	char *element = new char[str.size() + 1];
	std::memcpy(element, str.c_str(), str.size() + 1);
	return element;
}
