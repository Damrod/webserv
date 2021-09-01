#include <StringUtils.hpp>
#include <algorithm>
#include <cctype>

std::string	TrimString(const std::string &str, const std::string trim_chars) {
	const std::size_t	start_position = str.find_first_not_of(trim_chars);

	if (start_position == std::string::npos)
		return "";
	const std::size_t	end_position = str.find_last_not_of(trim_chars) + 1;
	return str.substr(start_position, end_position - start_position);
}

std::string	ToLowerString(const std::string &str) {
	std::string lowercase_str = str;
	std::transform(lowercase_str.begin(), lowercase_str.end(),
			lowercase_str.begin(), ::tolower);
	return lowercase_str;
}
