#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

#include <iterator>
#include <string>
#include <list>
#include <queue>
#include <vector>
#include <stack>
#include <parser/Lexer.hpp>
#include <parser/ParserAPI.hpp>

class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token, ParserAPI *config);

 private:
	void parse(void);
	class Data {
	public:
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
		void PushContext(const t_parsing_state &ctx) const;
		void PopContext(void) const;
		void NextEvent(void) const;
		t_parsing_state ParserLoopBack(void) const;
		t_token_type GetEvent(void) const;
		t_parsing_state GetState(void) const;
		const std::string &GetRawData(void) const;
		const std::string &GetErrorMessage(void) const;
		size_t GetLineNumber(void) const;
		size_t GetArgNumber(void) const;
		void IncrementArgNumber(void) const;
		void ResetArgNumber(void) const;

	private:
		const std::string &error_msg_;
		const size_t line_;
		const t_token_type event_;
		const t_parsing_state state_;
		const std::string &rawData_;
		Parser * const parser_;
		const t_parsing_state ctx_;
		ParserAPI *config_;
	};
	// ============= handlers ===================
	class StHandler {
	public:
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
	};
	StHandler handlers_;
	t_parsing_state ParserMainLoop(void);
	void PushContext_(const t_parsing_state &ctx);
	void PopContext_(void);
	t_parsing_state TopContext_(void) const;
	t_token_type NextEvent(void);
	size_t GetArgNumber(void);
	void IncrementArgNumber(void);
	void ResetArgNumber(void);
	std::stack<t_parsing_state> ctx_;
	const std::list<Token> &tokens_;
	ParserAPI *config_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_parsing_state state;
		t_token_type evt;
		t_parsing_state (Parser::StHandler::*apply)(const Data &data);
		std::string errormess;
	};
	static const s_trans transitions[13];
	size_t argNumber_;
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
