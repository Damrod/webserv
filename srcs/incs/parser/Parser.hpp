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
// #include <tuple>
#include <parser/Lexer.hpp>
#include <parser/ParserAPI.hpp>

namespace Parser {

class StatefulSet;
class Engine;

class StatelessSet : public Analyser {
 public:
	explicit StatelessSet(Engine *parser);
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
 private:
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
	t_parsing_state TopContext(void) const;
	t_token_type SkipEvent(void);
	size_t GetArgNumber(void);
	void IncrementArgNumber(void);
	void ResetArgNumber(void);

 private:
	static std::vector < Parser::s_trans > TransitionFactory_(void);
	StatelessSet handlers_;
	std::stack<t_parsing_state> ctx_;
	ParserAPI *config_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	static const std::vector < struct s_trans > transitions;
	size_t argNumber_;
};

class StatefulSet : public Analyser {
 public:
	StatefulSet(size_t line, t_token_type evt, t_parsing_state st,
		 const std::string &rawStatefulSet,
		 t_parsing_state ctx,
		 ParserAPI *config,
		 const std::string &error);
	StatefulSet(Engine * const parser, const std::string &error_msg);
	// this should probably take a std::string, not uint16_t
	void SetListenAddress(const std::string &svNameAddr) const;
	void AddServerName(const std::string &name) const;
	void SetRoot(const std::string &root) const;
	void AddIndex(const std::string &index) const;
	void AddAutoindex(const std::string &autoindex) const;
	void SetClientMaxSz(uint32_t size) const;
	void AddLocation(const std::string &name) const;
	void AddServer(void) const;
	void SkipEvent(void) const;
	t_token_type GetEvent(void) const;
	t_parsing_state GetState(void) const;
	const std::string &GetRawStatefulSet(void) const;
	const std::string &GetErrorMessage(void) const;
	size_t GetLineNumber(void) const;

 private:
	const std::string &error_msg_;
	const t_token_type event_;
	const t_parsing_state state_;
	const std::string &rawData_;
	const t_parsing_state ctx_;
	ParserAPI *config_;
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
