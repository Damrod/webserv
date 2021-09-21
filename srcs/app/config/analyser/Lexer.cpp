#include <parser/Lexer.hpp>

Lexer::~Lexer(void) {
	delete tokens_;
}

static void addStringLit(std::list<Token> *tokens, std::string *filebuff,
				  size_t *tokenend, size_t *line) {
	std::string token;
	char cmp;
	t_token_type type;

	cmp = (*filebuff)[0];
	type = (cmp == '\'' ?	Token::Type::T_STR_IMMEDIATE_T1:
							Token::Type::T_STR_IMMEDIATE_T0);
	*filebuff = filebuff->substr(1);
	*tokenend = filebuff->find(cmp, 0);
	if (*tokenend == filebuff->npos) {
		delete tokens;
		throw Analyser::SyntaxError("Unterminated quote in line", *line);
	}
	token = filebuff->substr(0, *tokenend);
	tokens->push_back(Token(token, type, *line));
	*line += std::count(token.begin(), token.end(), '\n');
	(*tokenend)++;
}

static void addPunct(std::list<Token> *tokens, char type,
					 size_t *tokenend, size_t line) {
	char			tmp[2];
	t_token_type	ttype;

	if (type == ';')
		ttype = Token::Type::T_END;
	else if (type == '{')
		ttype = Token::Type::T_SCOPE_OPEN;
	else if (type == '}')
		ttype = Token::Type::T_SCOPE_CLOSE;
	else
		throw Analyser::SyntaxError("Unexpected token near line", line);
	tmp[0] = type;
	tmp[1] = '\0';
	tokens->push_back(Token(tmp, ttype, line));
	*tokenend = 1;
}

std::list<Token> *Lexer::lexer(const std::string &fileBuff) {
	std::string filebuff = "{" + fileBuff + "}";  // add global scope
	std::list<Token> *tokens = new std::list<Token>;
	size_t line = 1;
	size_t tokenend = 0;
	std::string token;

	while (filebuff[0]) {
		filebuff = filebuff.substr(tokenend);
		for (; filebuff[0] && whitespace.find(filebuff[0], 0) != whitespace.npos
			 ; filebuff = filebuff.substr(1)) {
			if (filebuff[0] == '\n')
				line++;
		}
		if (filebuff[0] == '"' || filebuff[0] == '\'') {
			addStringLit(tokens, &filebuff, &tokenend, &line);
			continue;
		}
		if (validtokens.find(filebuff[0], 0) != validtokens.npos) {
			addPunct(tokens, filebuff[0], &tokenend, line);
			continue;
		}
		tokenend = filebuff.find_first_of(validtokens + whitespace, 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		if ((token = filebuff.substr(0, tokenend)) != "")
			tokens->push_back(Token(token, Token::Type::T_SYMBOL, line));
	}
	return tokens;
}

std::list<Token> *Lexer::GetTokens(void) const {
	return tokens_;
}

Lexer::Lexer(const std::string &filebuff)
	: validtokens("{};"),
	  whitespace(" \t\f\n\r\t\v\n"),
	  tokens_(NULL) {
	tokens_ = lexer(filebuff);
}
