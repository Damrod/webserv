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
			T_UNKNOWN = -1,  // {
			T_SCOPE_OPEN,  // {
			T_SCOPE_CLOSE,  // }
			T_SYMBOL,  // any valid keyword
			T_STR_IMMEDIATE_T0,  // "hello world"
			T_STR_IMMEDIATE_T1,  // 'hello world'
			T_INT_IMMEDIATE,  // -1213
			T_UINT_IMMEDIATE,  // 123123
			T_DBL_IMMEDIATE,  // 213.12313
			T_END,  //;
			T_INVALID
		};
		static std::string GetTokenTypeStr(enum e_id type);
	private:
		static const char *str_map[10];
	};
	class State {
	public:
		enum e_id {
			K_NONE = -1,
			K_EXIT = 0,
			K_INIT  = 1,
			K_SERVER  = 2,
			K_LISTEN = 3,		  // S pero no Loc
			K_SERVER_NAME = 4,	  // S pero no Loc
			K_ROOT = 5,
			K_CLIENT_MAX_BODY_SIZE = 6,
			K_ERROR_PAGE = 7,
			K_RETURN = 8,
			K_AUTOINDEX = 9,
			K_INDEX = 10,
			K_UPLOAD_STORE = 11,
			K_CGI_ASSIGN = 12,  // dependiendo de la extension del path
			K_LOCATION = 13,    // ejecuta la peticion con un binario distinto
			K_LIMIT_EXCEPT = 14,  // Solo location
			K_EXP_SEMIC = 15,
			K_EXP_KW = 16,
			K_LAST_INVALID_STATE  = 17
		};
		static std::string GetParsingStateTypeStr(enum e_id type);
		static enum e_id GetParsingStateTypeEnum(const std::string &data,
												 Type::e_id ttype);

	private:
		static const char *keyword_to_str[19];
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
