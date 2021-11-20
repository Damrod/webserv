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
char	Analyser::SyntaxError::lineerror_[500] = {};

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line) {
	// if this didn't need to not throw, we would use std strings or string
	// streams
	static const char inLine[] = " in line ";

	if (error.size() + strlen(inLine) + sizeof(utoaline_) + 1 >
	sizeof lineerror_) {
		std::memcpy(lineerror_, "Too long error message", 23);
		return;
	}
	Utoa_(line_, utoaline_);
	std::memcpy(lineerror_, error.c_str(), error.size());
	std::memcpy(&lineerror_[error.size()], inLine, strlen(inLine) + 1);
	std::memcpy(&lineerror_[error.size() + strlen(inLine)], utoaline_,
				strlen(utoaline_) + 1);
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	return lineerror_;
}
