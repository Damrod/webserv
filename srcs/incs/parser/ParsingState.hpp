#ifndef SRCS_INCS_PARSER_PARSINGSTATE_HPP_
#define SRCS_INCS_PARSER_PARSINGSTATE_HPP_

#include <iostream>
#include <map>
#include <string>
#include <parser/Token.hpp>

namespace Parser {

class State {
 public:
	enum e_id {
		K_NONE = -1,  // Wildcard state
		K_EXIT,
		K_INIT,
		K_SERVER,
		K_LISTEN,		  // S pero no Loc
		K_SERVER_NAME,	  // S pero no Loc
		K_ROOT,
		K_CLIENT_MAX_BODY_SIZE,
		K_ERROR_PAGE,
		K_RETURN,
		K_AUTOINDEX,
		K_INDEX,
		K_UPLOAD_STORE,
		K_CGI_ASSIGN,  // dependiendo de la extension del path
		K_LOCATION,    // ejecuta la peticion con un binario distinto
		K_LIMIT_EXCEPT,  // Solo location
		K_EXP_SEMIC,
		K_EXP_KW,
		K_LAST_INVALID_STATE
	};
	static std::string GetParsingStateTypeStr(enum e_id type);
	static enum e_id GetParsingStateTypeEnum(const Token &token);

 private:
	static std::map<const std::string, enum e_id> KeywordMapFactory_(void);
	static const std::map<const std::string, enum e_id> keyword_to_str;
};

}  // namespace Parser

typedef Parser::State::e_id t_parsing_state;

#endif  //  SRCS_INCS_PARSER_PARSINGSTATE_HPP_
