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

std::string	ToUpperString(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

char *DuplicateString(const std::string &str) {
	char *element = new char[str.size() + 1];
	std::memcpy(element, str.c_str(), str.size() + 1);
	return element;
}

std::string	DecodeUrl(const std::string &encoded_url) {
	std::string decoded_url;
	std::size_t length = encoded_url.size();

	decoded_url.reserve(encoded_url.size());
	for (std::size_t i = 0; i < length; ++i) {
		if (encoded_url[i] == '+') {
			decoded_url.append(1, ' ');
		} else if (encoded_url[i] == '%' &&
					i + 2 < length &&
					std::isxdigit(encoded_url[i + 1]) &&
					std::isxdigit(encoded_url[i + 2])) {
			std::istringstream is(encoded_url.substr(i + 1, 2));
			int value;
			is >> std::hex >> value;
			decoded_url.append(1, static_cast<char>(value));
			i += 2;
		} else {
			decoded_url.append(1, encoded_url[i]);
		}
	}
	return decoded_url;
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
