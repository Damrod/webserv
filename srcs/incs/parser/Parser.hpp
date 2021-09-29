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
#include <parser/ParsingEvents.hpp>


class Parser: public Analyser {
 public:
	Parser(const std::list<Token> &token, ParserAPI *config);

 private:
	void parse(void);
	class Data {
	public:
		const Token &current_;
		const std::string error_msg_;
		Parser * const parser_;
		std::stack<t_parsing_state> *ctx_;
		// this should probably take a std::string, not uint16_t
		void SetListenPort(uint16_t port) const;
		void SetListenAddress(uint32_t address) const;
		void AddServerName(const std::string &name) const;
		void SetRoot(const std::string &root) const;
		void AddIndex(const std::string &index) const;
		void AddAutoindex(const std::string &autoindex) const;
		void SetClientMaxSz(uint32_t size) const;
		void AddLocation(const std::string &name) const;
		void AddServer(void) const;
		Data(Parser * const parser, const std::string &error_msg);
	private:
		ParserAPI *config_;
	};
	// ============= handlers ===================
	class StHandler {
	public:
		static t_parsing_state SyntaxFailer(const Data &data);
		static t_parsing_state ServerNameHandler(const Data &data);
		static t_parsing_state InitHandler(const Data &data);
		static t_parsing_state SemicHandler(const Data &data);
		static t_parsing_state ExpKwHandlerClose(const Data &data);
		static t_parsing_state ExpKwHandlerKw(const Data &data);
		static t_parsing_state AutoindexHandler(const Data &data);
		static t_parsing_state LocationHandler(const Data &data);
		static t_parsing_state ServerHandler(const Data &data);
	};
	static t_parsing_state ParserMainLoop(Parser *parser);
	std::stack<t_parsing_state> ctx_;
	const std::list<Token> &tokens_;
	ParserAPI *config_;
	const std::list<Token>::const_iterator itb_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	struct s_trans {
		t_parsing_state state;
		t_Ev evt;
		t_parsing_state (*apply)(const Data &data);
		std::string errormess;
	};
	static const s_trans transitions[14];
};

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
