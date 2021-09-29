#ifndef SRCS_INCS_PARSER_TOKEN_HPP_
#define SRCS_INCS_PARSER_TOKEN_HPP_

#include <stdint.h>
#include <string>
#include <iostream>
#include <map>
#include <parser/ParsingEvents.hpp>

class Token {
 public:
	class Type {
	public:
		enum e_id {
			T_UNKNOWN = -1,  // Unknown, invalid
			T_SCOPE_OPEN,  // {
			T_SCOPE_CLOSE,  // }
			T_SEMICOLON,  //;
			T_WORD,
			T_INVALID
		};
		static std::string GetTokenTypeStr(enum e_id type);
	private:
		static const char *str_map[4];
	};
	class State {
	public:
		enum e_id {
			K_NONE = -1,
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
		static enum e_id GetParsingStateTypeEnum(const std::string &data,
												 Type::e_id ttype);

	private:
		static const char *keyword_to_str[18];
	};
	explicit Token(const std::string &data, Type::e_id type, size_t line);
	Type::e_id getType(void) const;
	std::string GetTokenTypeStr(void) const;
	const std::string &getRawData(void) const;
	size_t GetLine(void) const;
	t_Ev GetEvent(void) const;
	enum State::e_id GetState(void) const;

 protected:
	std::string data_;
	const Type::e_id type_;
	const size_t line_;
	const t_Ev event_;
	const enum State::e_id state_;
};

typedef enum Token::State::e_id t_parsing_state;
typedef Token::Type::e_id t_token_type;

#endif  // SRCS_INCS_PARSER_TOKEN_HPP_
