#ifndef SRCS_INCS_PARSER_PARSINGEVENTS_HPP_
#define SRCS_INCS_PARSER_PARSINGEVENTS_HPP_

#include <dirent.h>
#include <string>

class Token;

class ParsingEvents {
 public:
	enum e_id {
		EV_NONE = -1,
		KEYWORD,
		WORD,
		OPEN,
		CLOSE,
		SEMIC,
		INVALID
	};
	static enum e_id GetEvent(const Token &token);
};

typedef ParsingEvents::e_id t_Ev;

#include <parser/Token.hpp>

#endif  // SRCS_INCS_PARSER_PARSINGEVENTS_HPP_
