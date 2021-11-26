#include <parser/Analyser.hpp>

Analyser::Analyser(void) : line_(1) {}

Analyser::~Analyser(void) {}

char	Analyser::SyntaxError::lineerror_[200] = {};

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line) {
	// if this didn't need to not throw, we would use std strings or string
	// streams
	snprintf(lineerror_, sizeof(lineerror_), "%s in line %zu",
	error.c_str(), line);
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	return lineerror_;
}
