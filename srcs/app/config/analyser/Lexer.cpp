#include <parser/Lexer.hpp>

Lexer::~Lexer(void) {}

void Lexer::AddStringLit_(std::string *filebuff, size_t *tokenend) {
	std::string token;
	char cmp;

	cmp = (*filebuff)[0];
	*filebuff = filebuff->substr(1);
	*tokenend = filebuff->find(cmp, 0);
	if (*tokenend == filebuff->npos)
		throw Analyser::SyntaxError("Unterminated quote", line_);
	token = filebuff->substr(0, *tokenend);
	tokens_.push_back(Token(token, Token::Type::T_WORD, line_));
	line_ += std::count(token.begin(), token.end(), '\n');
	(*tokenend)++;
}

void Lexer::AddPunct_(char type, size_t *tokenend) {
	char			tmp[2];
	t_token_type	ttype;

	if (type == ';')
		ttype = Token::Type::T_SEMICOLON;
	else if (type == '{')
		ttype = Token::Type::T_SCOPE_OPEN;
	else if (type == '}')
		ttype = Token::Type::T_SCOPE_CLOSE;
	else
		throw Analyser::SyntaxError("Unexpected token", line_);
	tmp[0] = type;
	tmp[1] = '\0';
	tokens_.push_back(Token(tmp, ttype, line_));
	*tokenend = 1;
}

void Lexer::Lex_(const std::string &fileBuff) {
	std::string filebuff = "{" + fileBuff + "}";  // add global scope
	size_t tokenend = 0;
	std::string token;

	while (filebuff[0]) {
		filebuff = filebuff.substr(tokenend);
		for (; filebuff[0] && kWhitespace_.find(filebuff[0], 0) != kWhitespace_.npos
			 ; filebuff = filebuff.substr(1)) {
			if (filebuff[0] == '\n')
				line_++;
		}
		if (filebuff[0] == '"' || filebuff[0] == '\'') {
			AddStringLit_(&filebuff, &tokenend);
			continue;
		}
		if (kValidtokens_.find(filebuff[0], 0) != kValidtokens_.npos) {
			AddPunct_(filebuff[0], &tokenend);
			continue;
		}
		tokenend = filebuff.find_first_of(kValidtokens_ + kWhitespace_, 0);
		if (tokenend == filebuff.npos)
			tokenend = filebuff.size();
		if ((token = filebuff.substr(0, tokenend)) != "")
			tokens_.push_back(Token(token, Token::Type::T_WORD, line_));
	}
}

std::list<Token> Lexer::GetTokens(void) const {
	return tokens_;
}

Lexer::Lexer(const std::string &filebuff)
	: kValidtokens_("{};"),
	  kWhitespace_(" \f\n\r\t\v"),
	  tokens_() {
	line_ = 1;
	Lex_(filebuff);
}
