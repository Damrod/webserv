#include <parser/Token.hpp>

std::string Token::Type::GetTokenTypeStr(enum e_id type) {
	std::string retval("unknown");
	if (type > T_UNKNOWN && type < T_INVALID)
		return str_map[type];
	return retval;
}

const char *Token::Type::str_map[4] = {
	"t scope open",
	"t scope close",
	"t semicolon",
	"t word"
};

std::string Token::State::GetParsingStateTypeStr(enum e_id type) {
	std::string retval("");
	if (type > K_NONE && type < K_LAST_INVALID_STATE)
		return keyword_to_str[type];
	return retval;
}

t_parsing_state Token::State::GetParsingStateTypeEnum
(const std::string &data, t_token_type ttype) {
	if (ttype == Token::Type::T_SEMICOLON || ttype == Token::Type::T_SCOPE_OPEN
		|| ttype == Token::Type::T_SCOPE_CLOSE)
		return K_EXP_KW;
	for (uint8_t i = K_EXIT; i < K_LAST_INVALID_STATE; ++i) {
		if (keyword_to_str[i + 1] == data)
			return static_cast<t_parsing_state>(i);
	}
	return K_NONE;
}

const char *Token::State::keyword_to_str[18] = {
	"",
	"EXIT",
	"INIT",
	"server",
	"listen",
	"server_name",
	"root",
	"client_max_body_size",
	"error_page",
	"return",
	"autoindex",
	"index",
	"upload_store",
	"cgi_assign",
	"location",
	"limit_except",
	"EXPECT SEMICOLON",
	"EXPECT KEYWORD"
};

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line),
	  event_(ParsingEvents::GetEvent(*this)),
	  state_(State::GetParsingStateTypeEnum(data_, type_)) {
}

t_token_type Token::getType(void) const {
	return type_;
}

size_t Token::GetLine(void) const {
	return line_;
}

std::string Token::GetTokenTypeStr(void) const {
	return Token::Type::GetTokenTypeStr(type_);
}

const std::string &Token::getRawData(void) const {
	return data_;
}

t_Ev Token::GetEvent(void) const {
	return this->event_;
}

t_parsing_state Token::GetState(void) const {
	return this->state_;
}
