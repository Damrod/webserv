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
	std::list<Token> GetTokens(void) const;
 private:
	void lex(const std::string &fileBuff);
	void addPunct(char type, size_t *tokenend);
	void addStringLit(std::string *filebuff,
				  size_t *tokenend);
	const std::string		kValidtokens_;
	const std::string		kWhitespace_;
	std::list<Token>		tokens_;
};

#endif  // SRCS_INCS_PARSER_LEXER_HPP_
