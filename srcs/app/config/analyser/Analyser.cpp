#include <parser/Analyser.hpp>

Analyser::Analyser(void) {}

Analyser::~Analyser(void) {}

static void utoa(size_t value, char *dest) {
	if (value == 0) {
		std::memcpy(dest, "0", 2);
		return;
	}
	size_t i = 0;
	while (value) {
		dest[i] = '0' + (value % 10);
		value /= 10;
		i++;
	}
	dest[i] = '\0';
	size_t len = strlen(dest);
	for (i = 0; i < len / 2; ++i) {
		char tmp = dest[i];
		dest[i] = dest[len - i - 1];
		dest[len - i - 1] = tmp;
	}
}

char	Analyser::SyntaxError::itoaline_[12] = {};
char	Analyser::SyntaxError::lineerror_[100] = {};

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line) {
	utoa(line_, itoaline_);
	std::memcpy(lineerror_, error.c_str(), error.size());
	std::memcpy(&lineerror_[error.size()], " in line ", 9 + 1);
	std::memcpy(&lineerror_[error.size() + 9], itoaline_, strlen(itoaline_) + 1);
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	return lineerror_;
}
