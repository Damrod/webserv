#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

#include <arpa/inet.h>
#include <cerrno>
#include <iterator>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <stack>
#include <parser/Lexer.hpp>
#include <parser/ParserAPI.hpp>

namespace Parser {

class StatefulSet;
class Engine;

class StatelessSet : public Analyser {
 public:
	StatelessSet(Engine *parser, ParserAPI *config);
//  state handlers
	t_parsing_state SyntaxFailer(const StatefulSet &data);
	t_parsing_state ServerNameHandler(const StatefulSet &data);
	t_parsing_state InitHandler(const StatefulSet &data);
	t_parsing_state SemicHandler(const StatefulSet &data);
	t_parsing_state ExpKwHandlerClose(const StatefulSet &data);
	t_parsing_state ExpKwHandlerKw(const StatefulSet &data);
	t_parsing_state AutoindexHandler(const StatefulSet &data);
	t_parsing_state LocationHandler(const StatefulSet &data);
	t_parsing_state ServerHandler(const StatefulSet &data);
	t_parsing_state ListenHandler(const StatefulSet &data);
// setters
 private:
	void SetListenAddress(const std::string &svNameAddr,
						  t_parsing_state ctx) const;
	void AddServerName(const std::string &name, t_parsing_state ctx) const;
	void SetRoot(const std::string &root, t_parsing_state ctx) const;
	void AddIndex(const std::string &index, t_parsing_state ctx) const;
	void AddAutoindex(const std::string &autoindex, t_parsing_state ctx) const;
	void SetClientMaxSz(uint32_t size, t_parsing_state ctx) const;
	void AddLocation(const std::string &name, t_parsing_state ctx) const;
	void AddServer(t_parsing_state ctx) const;
	ParserAPI *config_;
	Engine *parser_;
};

typedef t_parsing_state (Parser::StatelessSet::*StateHandler)
	(const StatefulSet &data);

struct s_trans {
	t_parsing_state state;
	t_token_type evt;
	StateHandler apply;
	std::string errormess;
};


class Engine: public Analyser {
 public:
	Engine(const std::list<Token> &token, ParserAPI *config);
	t_parsing_state ParserMainLoop(void);
	void PushContext(const t_parsing_state &ctx);
	void PopContext(void);
	t_token_type SkipEvent(void);
	void IncrementArgNumber(void);
	void ResetArgNumber(void);

 private:
	virtual std::vector < Parser::s_trans > TransitionFactory_(void);
	StatelessSet handlers_;
	std::stack<t_parsing_state> ctx_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	const std::vector < struct s_trans > transitions_;
	size_t argNumber_;
};

class StatefulSet : public Analyser {
 public:
	StatefulSet(size_t line,
				t_token_type evt,
				t_parsing_state st,
				const std::string &rawData,
				t_parsing_state ctx,
				const std::string &error,
				size_t argNumber);
	t_token_type GetEvent(void) const;
	t_parsing_state GetCtx(void) const;
	t_parsing_state GetState(void) const;
	const std::string &GetRawData(void) const;
	const std::string &GetErrorMessage(void) const;
	size_t GetLineNumber(void) const;
	size_t GetArgNumber(void) const;

 private:
	const std::string &error_msg_;
	const t_token_type event_;
	const t_parsing_state state_;
	const std::string &rawData_;
	const t_parsing_state ctx_;
	const size_t argNumber_;
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
