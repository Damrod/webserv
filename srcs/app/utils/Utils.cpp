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

std::string	PathExtension(const std::string &path) {
	const std::size_t extension_position = path.rfind(".");
	if (extension_position == std::string::npos || extension_position < 2) {
		return "";
	}
	const std::size_t	last_dir_position = path.rfind("/");
	if (last_dir_position == std::string::npos) {
		return path.substr(extension_position + 1);
	}
	if (last_dir_position < extension_position - 1) {
		const std::string last_path_part = path.substr(last_dir_position + 1);
		if (last_path_part != "." && last_path_part != "..") {
			return path.substr(extension_position + 1);
		}
	}
	return "";
}

bool	IsExecutable(const std::string &path) {
	return access(path.c_str(), X_OK) == 0;
}
