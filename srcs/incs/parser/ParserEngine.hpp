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
	State::e_id ParserMainLoop(void);
	void PushContext(const State::e_id &ctx);
	void PopContext(void);
	Event::e_id SkipEvent(void);
	const std::vector<std::string> &GetArgs(void) const;
	void IncrementArgNumber(const std::string &arg);
	void ResetArgNumber(void);

 private:
	virtual std::vector < Parser::s_trans > TransitionFactory_(void);
	StatelessSet handlers_;
	std::stack<State::e_id> ctx_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	const std::vector < struct s_trans > transitions_;
	std::vector<std::string> args_;
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSERENGINE_HPP_
