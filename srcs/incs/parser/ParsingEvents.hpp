#ifndef SRCS_INCS_PARSER_PARSINGEVENTS_HPP_
#define SRCS_INCS_PARSER_PARSINGEVENTS_HPP_

#include <dirent.h>
#include <string>

class Token;

class ParsingEvents {
 public:
	enum e_id {
		EV_NONE = -1,
		IP = 0,
		PORT = 1,
		IP_PORT = 2,
		DIR = 3,
		SIZE = 4,
		URI = 5,
		URL = 6,
		ERROR_CODE = 7,
		ON_OFF = 8,
		FILE = 9,
		METHOD = 10,
		OPEN = 11,
		CLOSE = 12,
		SEMIC = 13,
		KEYWORD = 14,
		INVALID
	};
	static enum e_id GetEvent(const Token &token);
};

typedef ParsingEvents::e_id t_Ev;

#include <parser/Token.hpp>

#endif  // SRCS_INCS_PARSER_PARSINGEVENTS_HPP_
