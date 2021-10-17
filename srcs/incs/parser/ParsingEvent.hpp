#ifndef SRCS_INCS_PARSER_PARSINGEVENT_HPP_
#define SRCS_INCS_PARSER_PARSINGEVENT_HPP_

#include <stack>
#include <vector>
#include <parser/ParsingState.hpp>

namespace Parser {

class Event {
 public:
	enum e_id {
		EVT_NONE = -1,  // Wildcard event
		EVT_SCOPE_OPEN,  // {
		EVT_SCOPE_CLOSE,  // }
		EVT_SEMICOLON,  //;
		EVT_WORD,
		EVT_INVALID
	};
	static e_id GetEventTypeEnum(const Token &token);
};

}  // namespace Parser

typedef Parser::Event::e_id t_evt;

#endif  // SRCS_INCS_PARSER_PARSINGEVENT_HPP_
