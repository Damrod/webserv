#include <parser/Analyser.hpp>

Analyser::Analyser(void) : line_(1) {}

Analyser::~Analyser(void) {}

void Analyser::SyntaxError::Utoa_(size_t value, char *dest) {
// normally this would be done with sprintf, but we can't use printf stuff
	if (value == 0) {
		std::memcpy(dest, "0", 2);
		return;
	}
	size_t i = 0;
	for (; value != 0; ++i) {
		dest[i] = '0' + (value % 10);
		value /= 10;
	}
	dest[i] = '\0';
	size_t len = strlen(dest);
	for (i = 0; i < len / 2; ++i) {
		char tmp = dest[i];
		dest[i] = dest[len - i - 1];
		dest[len - i - 1] = tmp;
	}
}

char	Analyser::SyntaxError::utoaline_[12] = {};
char	Analyser::SyntaxError::lineerror_[100] = {};

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line) {
	// if this didn't need to not throw, we would use std strings or string
	// streams. If we could use printfs, we would use
	// snprintf(lineerror_, sizeof(lineerror_), "%s in line %zu",
	// error.c_str(), line);
	static const char inLine[] = " in line ";

	Utoa_(line_, utoaline_);
	size_t size = std::min(error.size(), sizeof(lineerror_) - 1);
	std::memcpy(lineerror_, error.c_str(), size);
	lineerror_[size] = '\0';
	size_t size2 = std::min(strlen(inLine), sizeof(lineerror_) - size - 1);
	std::memcpy(&lineerror_[size], inLine, size2);
	lineerror_[size + size2] = '\0';
	size_t size3 = std::min(strlen(utoaline_) + 1, sizeof(lineerror_) - size
	- size2 - 1);
	std::memcpy(&lineerror_[size + size2], utoaline_,
				size3);
	lineerror_[size + size2 + size3] = '\0';
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	return lineerror_;
}
