#include <parser/ParsingState.hpp>

struct Parser::State::str_to_state Parser::State::str_to_state_map_[13] = {
	{"server", Parser::State::K_SERVER},
	{"listen", Parser::State::K_LISTEN},
	{"server_name", Parser::State::K_SERVER_NAME},
	{"root", Parser::State::K_ROOT},
	{"client_max_body_size",
	 Parser::State::K_CLIENT_MAX_BODY_SIZE},
	{"error_page", Parser::State::K_ERROR_PAGE},
	{"return", Parser::State::K_RETURN},
	{"autoindex", Parser::State::K_AUTOINDEX},
	{"index", Parser::State::K_INDEX},
	{"upload_store", Parser::State::K_UPLOAD_STORE},
	{"cgi_assign", Parser::State::K_CGI_ASSIGN},
	{"location", Parser::State::K_LOCATION},
	{"limit_except", Parser::State::K_LIMIT_EXCEPT},
};

std::string Parser::State::GetParsingStateTypeStr(t_parsing_state type) {
	for (std::size_t i = 0; i < (sizeof (str_to_state_map_)
	/ sizeof (str_to_state_map_[0])); ++i) {
		if(type == str_to_state_map_[i].id) {
			return str_to_state_map_[i].name;
		}
	}
	return "";
}

t_parsing_state Parser::State::GetParsingStateTypeEnum(const Token &token) {
	if (token.GetType() == Token::Type::T_SEMICOLON
		|| token.GetType() == Token::Type::T_SCOPE_OPEN
		|| token.GetType() == Token::Type::T_SCOPE_CLOSE)
		return K_EXP_KW;
	for (std::size_t i = 0; i < (sizeof (str_to_state_map_)
	/ sizeof (str_to_state_map_[0])); ++i) {
		if (token.GetRawData() == str_to_state_map_[i].name) {
			return str_to_state_map_[i].id;
		}
	}
	return Parser::State::K_NONE;
}
