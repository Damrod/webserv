#ifndef SRCS_INCS_PARSER_LEXER_HPP_
#define SRCS_INCS_PARSER_LEXER_HPP_

# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <algorithm>
# include <cctype>
# include <list>
# include <map>
# include <vector>
# include <sstream>
# include <parser/Analyser.hpp>
# include <parser/Token.hpp>

class Lexer : public Analyser {
 public:
	explicit Lexer(const std::string &filebuff);
	virtual ~Lexer(void);
	std::list<Token> *lexer(const std::string &fileBuff);
	std::list<Token> *GetTokens(void) const;
 private:
	const std::string		validtokens;
	const std::string		whitespace;
	std::list<Token>	*tokens_;
};

std::ostream &operator<<(std::ostream &o, Lexer const &i);

#endif  // SRCS_INCS_PARSER_LEXER_HPP_
