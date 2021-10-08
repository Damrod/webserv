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
#include <tuple>
#include <parser/Lexer.hpp>
#include <parser/ParserAPI.hpp>

class Data;
class Parser;

class StHandler : public Analyser {
 public:
	explicit StHandler(Parser *parser);
	t_parsing_state SyntaxFailer(const Data &data);
	t_parsing_state ServerNameHandler(const Data &data);
	t_parsing_state InitHandler(const Data &data);
	t_parsing_state SemicHandler(const Data &data);
	t_parsing_state ExpKwHandlerClose(const Data &data);
	t_parsing_state ExpKwHandlerKw(const Data &data);
	t_parsing_state AutoindexHandler(const Data &data);
	t_parsing_state LocationHandler(const Data &data);
	t_parsing_state ServerHandler(const Data &data);
	t_parsing_state ListenHandler(const Data &data);
 private:
	Parser *parser_;
};

struct s_trans {
	t_parsing_state state;
	t_token_type evt;
	t_parsing_state (StHandler::*apply)(const Data &data);
	std::string errormess;
};

typedef t_parsing_state (StHandler::*StateHandler)(const Data &data);

class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token, ParserAPI *config);
	t_parsing_state ParserMainLoop(void);
	void PushContext_(const t_parsing_state &ctx);
	void PopContext_(void);
	t_parsing_state TopContext_(void) const;
	t_token_type SkipEvent(void);
	size_t GetArgNumber(void);
	void IncrementArgNumber(void);
	void ResetArgNumber(void);

 private:
	// ============= handlers ===================
	StHandler handlers_;
	std::stack<t_parsing_state> ctx_;
	ParserAPI *config_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	static const s_trans transitions[18];
	static const std::vector < std::tuple <
		t_parsing_state,
		t_token_type,
		StateHandler > > trans;
	size_t argNumber_;
};

class Data : public Analyser {
 public:
	Data(size_t line, t_token_type evt, t_parsing_state st,
		 const std::string &rawData,
		 t_parsing_state ctx,
		 ParserAPI *config,
		 const std::string &error);
	Data(Parser * const parser, const std::string &error_msg);
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
	const std::string &GetRawData(void) const;
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

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
