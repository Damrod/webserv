#include <parser/Parser.hpp>

const Parser::State::t_kw2str Parser::State::kw2strmap[14] = {
		{.state = Parser::State::K_SERVER,
		.data = "server"},
		{.state = Parser::State::K_LISTEN,
		.data = "listen"},
		{.state = Parser::State::K_SERVER_NAME,
		.data = "server_name"},
		{.state = Parser::State::K_ROOT,
		.data = "root"},
		{.state = Parser::State::K_CLIENT_MAX_BODY_SIZE,
		.data = "client_max_body_size"},
		{.state = Parser::State::K_ERROR_PAGE,
		.data = "error_page"},
		{.state = Parser::State::K_RETURN,
		.data = "return"},
		{.state = Parser::State::K_AUTOINDEX,
		.data = "autoindex"},
		{.state = Parser::State::K_INDEX,
		.data = "index"},
		{.state = Parser::State::K_UPLOAD_STORE,
		.data = "upload_store"},
		{.state = Parser::State::K_SERVER_NAME,
		.data = "server_name"},
		{.state = Parser::State::K_CGI_ASSIGN,
		.data = "cgi_assign"},
		{.state = Parser::State::K_LOCATION,
		.data = "location"},
		{.state = Parser::State::K_LIMIT_EXCEPT,
		.data = "limit_except"}};

std::string Parser::State::GetParsingStateTypeStr(enum e_id type) {
	for (size_t i = 0; i < sizeof(kw2strmap)/sizeof(kw2strmap[0]); ++i) {
		if(type == kw2strmap[i].state)
			return kw2strmap[i].data;
	}
	return "";
}

t_parsing_state Parser::State::GetParsingStateTypeEnum(const Token &token) {
	if (token.getType() == Token::Type::T_SEMICOLON
		|| token.getType() == Token::Type::T_SCOPE_OPEN
		|| token.getType() == Token::Type::T_SCOPE_CLOSE)
		return K_EXP_KW;
	for (size_t i = 0; i < sizeof(kw2strmap)/sizeof(kw2strmap[0]); ++i) {
		if(token.getRawData() == kw2strmap[i].data)
			return kw2strmap[i].state;
	}
	return Parser::State::K_NONE;
}
