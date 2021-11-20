#ifndef SRCS_INCS_PARSER_ANALYSER_HPP_
#define SRCS_INCS_PARSER_ANALYSER_HPP_

# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <sstream>

class Analyser {
 public:
	Analyser(void);
	virtual ~Analyser(void);
	class SyntaxError : public std::exception {
	public:
		virtual ~SyntaxError(void) throw();
		explicit SyntaxError(const std::string &error, size_t line);
		virtual const char *what() const throw();
	private:
	  	static void Utoa_(size_t value, char *dest);
		static char		utoaline_[12];
		static char		lineerror_[500];
		size_t		line_;
	};

 protected:
	size_t					line_;
};

std::ostream &operator<<(std::ostream &o, Analyser const &i);

#endif  // SRCS_INCS_PARSER_ANALYSER_HPP_
