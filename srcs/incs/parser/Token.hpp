#ifndef SRCS_INCS_PARSER_TOKEN_HPP_
#define SRCS_INCS_PARSER_TOKEN_HPP_

#include <stdint.h>
#include <string>
#include <iostream>
#include <map>

class Token {
 public:
	class Type {
	public:
		enum e_id {
			T_NONE = -1,  // Unknown, invalid
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
	explicit Token(const std::string &data, Type::e_id type, size_t line);
	Type::e_id getType(void) const;
	std::string GetTokenTypeStr(void) const;
	const std::string &getRawData(void) const;
	size_t GetLine(void) const;

 protected:
	std::string data_;
	const Type::e_id type_;
	const size_t line_;
};

typedef Token::Type::e_id t_token_type;

#endif  // SRCS_INCS_PARSER_TOKEN_HPP_
