#ifndef SRCS_INCS_PARSER_PARSER_HPP_
#define SRCS_INCS_PARSER_PARSER_HPP_

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <iterator>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
#include <parser/Analyser.hpp>
#include <parser/Token.hpp>

struct ServerConfig;

namespace Parser {

template <typename T>
std::string toStrIndented(uint8_t level, const std::string &key,
const T &value) {
	std::ostringstream str;
	for (size_t i = 0; i < level; ++i)
		str << "\t";
	str << key << " : " << value << std::endl;
	return str.str();
}

template <typename T, typename U>
std::string MapToStrIndented(uint8_t level, const std::string &title,
std::map<T, U>map, const std::string &key, const std::string &value) {
	std::stringstream o;
	for (size_t i = 0; i < level; ++i)
		o << "\t";
	o << title << " : \n";
	for(typename std::map<T, U>::const_iterator iter = map.begin();
		iter != map.end();
		++iter) {
		for (size_t i = 0; i < static_cast<size_t>(level + 1); ++i)
			o << "\t";
		o << key << ": " << iter->first << ", "<< value << ":"
		  << iter->second << "\n";
	}
	return o.str();
}

class ParserAPI : public Analyser {
 private:
	std::vector<ServerConfig>	*servers_settings_;
	bool canAddServer(uint32_t address, uint16_t port);
	bool canAddLocation(const std::string &path);

 public:
	explicit ParserAPI(std::vector<ServerConfig> *server_settings);
	std::vector<ServerConfig> &GetServersSettings(void);
	void SetServersSettings(std::vector<ServerConfig> *server_settings);
	virtual ~ParserAPI(void) {}
	void SetListenAddress(uint32_t address, uint16_t port, t_parsing_state ctx,
						  size_t line);
	void AddServerName(const std::vector<std::string> &args, t_parsing_state ctx,
					   size_t line);
	void SetRoot(const std::string &root, t_parsing_state ctx, size_t line);
	void AddIndex(const std::string &index, t_parsing_state ctx, size_t line);
	void AddAutoindex(bool autoindex, t_parsing_state ctx, size_t line);
	void SetClientMaxSz(uint32_t size, t_parsing_state ctx, size_t line);
	void AddErrorPage(uint16_t code, const std::string &uri,
					  t_parsing_state ctx, size_t line);
	void AddCgiAssign(const std::string &extension,
								const std::string &binaryHandlerPath,
					  t_parsing_state ctx, size_t line);
	void AddServer(t_parsing_state ctx, size_t line);
	void AddLocation(const std::string &path, t_parsing_state ctx, size_t line);
};

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

class Helpers {
 public:
	static bool ParseIpAddressPort(const std::string &input,
								   std::string *errorThrow,
								   uint16_t *port, uint32_t *address);
	static bool isKwAllowedInCtx(t_parsing_state kw, t_parsing_state ctx);
};

class Engine: public Analyser {
 public:
	Engine(const std::list<Token> &token, ParserAPI *config);
	t_parsing_state ParserMainLoop(void);
	void PushContext(const t_parsing_state &ctx);
	void PopContext(void);
	t_token_type SkipEvent(void);
	const std::vector<std::string> &GetArgs(void) const;
	void IncrementArgNumber(const std::string &arg);
	void ResetArgNumber(void);

 private:
	virtual std::vector < Parser::s_trans > TransitionFactory_(void);
	StatelessSet handlers_;
	std::stack<t_parsing_state> ctx_;
	const std::list<Token>::const_iterator ite_;
	std::list<Token>::const_iterator itc_;
	const std::vector < struct s_trans > transitions_;
	std::vector<std::string> args_;
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
