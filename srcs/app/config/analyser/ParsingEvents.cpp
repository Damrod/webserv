#include <parser/ParsingEvents.hpp>

static bool isKwLoc(const std::string &str, t_token_type type) {
	t_parsing_state kw = Token::State::GetParsingStateTypeEnum(str, type);
	if ((Token::State::K_INIT < kw
		 && kw < Token::State::K_LAST_INVALID_STATE)
		&& kw != Token::State::K_LISTEN
		&& kw != Token::State::K_SERVER_NAME)
		return true;
	return false;
}

static bool isKwServ(const std::string &str, t_token_type type) {
	t_parsing_state kw = Token::State::GetParsingStateTypeEnum(str, type);
	if ((Token::State::K_INIT < kw
		 && kw < Token::State::K_LAST_INVALID_STATE)
		&& kw != Token::State::K_LIMIT_EXCEPT)
		return true;
	return false;
}

t_Ev ParsingEvents::GetEvent(const Token &token) {
	const std::string &str = token.getRawData();
	t_token_type type = token.getType();
	if (type == Token::Type::T_SCOPE_CLOSE)
		return CLOSE;
	else if (type == Token::Type::T_SEMICOLON)
		return SEMIC;
	else if (type == Token::Type::T_SCOPE_OPEN)
		return OPEN;
	else if (isKwServ(str, type) || isKwLoc(str, type))
		return KEYWORD;
	else
		return WORD;  // we still find first the serv ones,
}
