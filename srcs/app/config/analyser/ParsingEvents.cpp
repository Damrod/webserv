#include <parser/ParsingEvents.hpp>

static bool isIp(const std::string &str) {
	return str == "0.0.0.0";
}

static bool isPort(const std::string &str) {
	return str == "80" || str == "8080";
}

static bool isIpPort(const std::string &str) {
	return str == "0.0.0.0:80" || str == "0.0.0.0:8080";
}

static bool isDir(const std::string &str) {
	DIR *d = opendir(str.c_str());
	void *ret = d;
	if (d)
		closedir(d);
	return ret != NULL;
}

static bool isSize(const std::string &str) {
	return str == "1m";  // configure to use only bytes
}

static bool isUri(const std::string &str) {
	return str == "/";
}

static bool isUrl(const std::string &str) {
	return str == "42agv.com";
}

static bool isError(const std::string &str) {
	return str == "404";
}

static bool isFile(const std::string &str) {
	return str == "index.html";
}

static bool isMethod(const std::string &str) {
	return str == "POST";
}

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
	t_Ev retval = INVALID;
	const std::string &str = token.getRawData();
	t_token_type type = token.getType();
	if (type == Token::Type::T_SCOPE_CLOSE)
		return CLOSE;
	else if (type == Token::Type::T_END)
		return SEMIC;
	else if (type == Token::Type::T_SCOPE_OPEN)
		return OPEN;
	else if (str == "on" || str == "off")
		return ON_OFF;
	else if (isIp(str))
		return IP;
	else if (isPort(str))
		return PORT;
	else if (isIpPort(str))
		return IP_PORT;
  // else if (isDir(str))
  // return DIR; URI is also dir, but dir is before, so it doesnt enter
	else if (isSize(str))
		return SIZE;
	else if (isUri(str) || isDir(str))
		return URI;
	else if (isUrl(str))
		return URL;
	else if (isError(str))
		return ERROR_CODE;
	else if (isFile(str))
		return FILE;
	else if (isMethod(str))
		return METHOD;
	else if (isKwServ(str, type) || isKwLoc(str, type))
  // otherwise, when we're in loc,
		return KEYWORD;  // we still find first the serv ones,
	return retval;
}
