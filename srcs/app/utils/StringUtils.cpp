#include <StringUtils.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>

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

std::string	UnsignedToStr(std::size_t nbr) {
	std::stringstream out;
	out << nbr;
	return out.str();
}
