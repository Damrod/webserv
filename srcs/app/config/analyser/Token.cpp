#include <parser/Token.hpp>

std::string Token::Type::GetTokenTypeStr(enum e_id type) {
	std::string retval("NONE");
	if (type >= 0 && type < T_INVALID)
		return str_map[type];
	return retval;
}

const char *Token::Type::str_map[4] = {
	"t scope open",
	"t scope close",
	"t semicolon",
	"t word"
};

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line) {
}

t_token_type Token::GetType(void) const {
	return type_;
}

size_t Token::GetLine(void) const {
	return line_;
}

std::string Token::GetTokenTypeStr(void) const {
	return Token::Type::GetTokenTypeStr(type_);
}

const std::string &Token::GetRawData(void) const {
	return data_;
}
