#include <parser/ParsingState.hpp>

std::map<const std::string, t_parsing_state>
								Parser::State::KeywordMapFactory_(void) {
	std::map<const std::string, enum e_id> ret;
	ret.insert(std::make_pair("server", Parser::State::K_SERVER));
	ret.insert(std::make_pair("listen", Parser::State::K_LISTEN));
	ret.insert(std::make_pair("server_name", Parser::State::K_SERVER_NAME));
	ret.insert(std::make_pair("root", Parser::State::K_ROOT));
	ret.insert(std::make_pair("client_max_body_size",
							  Parser::State::K_CLIENT_MAX_BODY_SIZE));
	ret.insert(std::make_pair("error_page", Parser::State::K_ERROR_PAGE));
	ret.insert(std::make_pair("return", Parser::State::K_RETURN));
	ret.insert(std::make_pair("autoindex", Parser::State::K_AUTOINDEX));
	ret.insert(std::make_pair("index", Parser::State::K_INDEX));
	ret.insert(std::make_pair("upload_store", Parser::State::K_UPLOAD_STORE));
	ret.insert(std::make_pair("cgi_assign", Parser::State::K_CGI_ASSIGN));
	ret.insert(std::make_pair("location", Parser::State::K_LOCATION));
	ret.insert(std::make_pair("limit_except", Parser::State::K_LIMIT_EXCEPT));
	return ret;
}


std::string Parser::State::GetParsingStateTypeStr(t_parsing_state type) {
	std::map<const std::string, t_parsing_state>::const_iterator itb =
		keyword_to_str.begin();
	for (; itb != keyword_to_str.end(); ++itb) {
		if(type == itb->second)
			return itb->first;
	}
	return "";
}

const std::map<const std::string, t_parsing_state> Parser::State::
keyword_to_str = Parser::State::KeywordMapFactory_();

t_parsing_state Parser::State::GetParsingStateTypeEnum(const Token &token) {
	if (token.GetType() == Token::Type::T_SEMICOLON
		|| token.GetType() == Token::Type::T_SCOPE_OPEN
		|| token.GetType() == Token::Type::T_SCOPE_CLOSE)
		return K_EXP_KW;
	std::map<const std::string, t_parsing_state>::const_iterator found =
		keyword_to_str.find(token.GetRawData());
	if (found != keyword_to_str.end())
		return found->second;
	return Parser::State::K_NONE;
}
