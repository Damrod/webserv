#include <parser/Analyser.hpp>

Analyser::Analyser(void) {}

Analyser::~Analyser(void) {}

Analyser::SyntaxError::SyntaxError(const std::string &error, size_t line)
	: line_(line) {
	std::ostringstream str;
	str << error << " " << line_;
	lineerr_ = str.str();
}

Analyser::SyntaxError::~SyntaxError() throw() {}

const char *Analyser::SyntaxError::what() const throw() {
	return lineerr_.c_str();
}
