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

const Token::State::t_kw2str Token::State::kw2strmap[14] = {
		{.state = Token::State::K_SERVER,
		.data = "server"},
		{.state = Token::State::K_LISTEN,
		.data = "listen"},
		{.state = Token::State::K_SERVER_NAME,
		.data = "server_name"},
		{.state = Token::State::K_ROOT,
		.data = "root"},
		{.state = Token::State::K_CLIENT_MAX_BODY_SIZE,
		.data = "client_max_body_size"},
		{.state = Token::State::K_ERROR_PAGE,
		.data = "error_page"},
		{.state = Token::State::K_RETURN,
		.data = "return"},
		{.state = Token::State::K_AUTOINDEX,
		.data = "autoindex"},
		{.state = Token::State::K_INDEX,
		.data = "index"},
		{.state = Token::State::K_UPLOAD_STORE,
		.data = "upload_store"},
		{.state = Token::State::K_SERVER_NAME,
		.data = "server_name"},
		{.state = Token::State::K_CGI_ASSIGN,
		.data = "cgi_assign"},
		{.state = Token::State::K_LOCATION,
		.data = "location"},
		{.state = Token::State::K_LIMIT_EXCEPT,
		.data = "limit_except"}};

std::string Token::State::GetParsingStateTypeStr(enum e_id type) {
	for (size_t i = 0; i < sizeof(kw2strmap)/sizeof(kw2strmap[0]); ++i) {
		if(type == kw2strmap[i].state)
			return kw2strmap[i].data;
	}
	return "";
}

t_parsing_state Token::State::GetParsingStateTypeEnum
(const std::string &data, t_token_type ttype) {
	if (ttype == Token::Type::T_SEMICOLON || ttype == Token::Type::T_SCOPE_OPEN
		|| ttype == Token::Type::T_SCOPE_CLOSE)
		return K_EXP_KW;
	for (size_t i = 0; i < sizeof(kw2strmap)/sizeof(kw2strmap[0]); ++i) {
		if(data == kw2strmap[i].data)
			return kw2strmap[i].state;
	}
	return Token::State::K_NONE;
}

Token::Token(const std::string &data, t_token_type type, size_t line)
	: data_(data),
	  type_(type),
	  line_(line),
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

t_parsing_state Token::GetState(void) const {
	return this->state_;
}
