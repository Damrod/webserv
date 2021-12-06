#ifndef SRCS_INCS_PARSER_PARSERENGINE_HPP_
#define SRCS_INCS_PARSER_PARSERENGINE_HPP_

#include <string>
#include <list>
#include <stack>
#include <vector>
#include <parser/ParserWrapper.hpp>
#include <parser/Analyser.hpp>
#include <parser/ParserStatelessSet.hpp>
#include <parser/ParsingState.hpp>
#include <parser/ParsingEvent.hpp>


namespace Parser {

class Engine: public Analyser {
 public:
	Engine(const std::list<Token> &token, Wrapper *config);
	virtual ~Engine(void);
	State::e_id ParserMainLoop(void);
	void PushContext(const State::e_id &ctx);
	Parser::State::e_id PopContext(void);
	Event::e_id SkipEvent(void);
	const std::vector<std::string> &GetArgs(void) const;
	void IncrementArgNumber(const std::string &arg);
	void ResetArgNumber(void);

 private:
	StatelessSet handlers_;
	std::stack<State::e_id> ctx_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	std::vector<std::string> args_;
	static const Parser::s_trans transitions_[35];
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSERENGINE_HPP_
