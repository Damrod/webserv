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
	size_t i;
	for (i = 0; i < level; ++i)
		o << "\t";
	o << title << " : \n";
	i = 0;
	for(typename std::map<T, U>::const_iterator iter = map.begin();
		iter != map.end();
		++iter, ++i) {
		for (size_t j = 0; j < static_cast<size_t>(level + 1); ++j)
			o << "\t";
		o << i << ": " << key << ": " << iter->first << ", "<< value << ":"
		  << iter->second << "\n";
	}
	return o.str();
}

class State {
 public:
	enum e_id {
		K_NONE = -1,
		K_EXIT,
		K_INIT,
		K_SERVER,
		K_LISTEN,		  // S pero no Loc
		K_SERVER_NAME,	  // S pero no Loc
		K_ROOT,
		K_CLIENT_MAX_BODY_SIZE,
		K_ERROR_PAGE,
		K_RETURN,
		K_AUTOINDEX,
		K_INDEX,
		K_UPLOAD_STORE,
		K_CGI_ASSIGN,  // dependiendo de la extension del path
		K_LOCATION,    // ejecuta la peticion con un binario distinto
		K_LIMIT_EXCEPT,  // Solo location
		K_EXP_SEMIC,
		K_EXP_KW,
		K_LAST_INVALID_STATE
	};
	static std::string GetParsingStateTypeStr(enum e_id type);
	static enum e_id GetParsingStateTypeEnum(const Token &token);
	typedef struct keyword_to_str {
		enum e_id state;
		std::string	data;
	} t_kw2str;

 private:
	static std::map<std::string, enum e_id> KeywordMapFactory_(void);
	static const std::map<std::string, enum e_id> keyword_to_str;
};

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
	void SetListenAddress(uint32_t address, uint16_t port,
						  State::e_id ctx, size_t line);
	void AddServerName(const std::vector<std::string> &args,
					   State::e_id ctx, size_t line);
	void SetRoot(const std::string &root, State::e_id ctx, size_t line);
	void AddIndex(const std::string &index, State::e_id ctx,
				  size_t line);
	void AddAutoindex(bool autoindex, State::e_id ctx, size_t line);
	void SetClientMaxSz(uint32_t size, State::e_id ctx, size_t line);
	void AddErrorPage(uint16_t code, const std::string &uri,
					  State::e_id ctx, size_t line);
	void AddCgiAssign(const std::string &extension,
					  const std::string &binaryHandlerPath,
					  State::e_id ctx, size_t line);
	void AddServer(State::e_id ctx, size_t line);
	void AddLocation(const std::string &path, State::e_id ctx,
					 size_t line);
	void AddReturn(uint16_t status, const std::string &url,
				   State::e_id ctx, size_t line);
	void AddUploadStore(const std::string &store,
						State::e_id ctx, size_t line);
	void AddLimitExcept(const std::vector<std::string> &httpMethods,
						State::e_id ctx, size_t line);
};

class StatefulSet;
class Engine;

class StatelessSet : public Analyser {
 public:
	StatelessSet(Engine *parser, ParserAPI *config);
//  state handlers
	State::e_id SyntaxFailer(const StatefulSet &data);
	State::e_id ServerNameHandler(const StatefulSet &data);
	State::e_id ServerNameHandlerSemic(const StatefulSet &data);
	State::e_id InitHandler(const StatefulSet &data);
	State::e_id SemicHandler(const StatefulSet &data);
	State::e_id ExpKwHandlerClose(const StatefulSet &data);
	State::e_id ExpKwHandlerKw(const StatefulSet &data);
	State::e_id AutoindexHandler(const StatefulSet &data);
	State::e_id LocationHandler(const StatefulSet &data);
	State::e_id ServerHandler(const StatefulSet &data);
	State::e_id ListenHandler(const StatefulSet &data);
	State::e_id ErrorPageHandler(const StatefulSet &data);
	State::e_id CgiAssignHandler(const StatefulSet &data);
	State::e_id RootHandler(const StatefulSet &data);
	State::e_id IndexHandler(const StatefulSet &data);
	State::e_id ClientMaxBodySizeHandler(const StatefulSet &data);
	State::e_id ReturnHandler(const StatefulSet &data);
	State::e_id LimitExceptHandler(const StatefulSet &data);
	State::e_id LimitExceptHandlerSemic(const StatefulSet &data);
	State::e_id UploadStoreHandler(const StatefulSet &data);

 private:
	// helpers
	bool ParserErrorPage_(const std::vector<std::string> &input,
						 uint16_t *code, std::string *uri);
	bool ParseIpAddressPort_(const std::string &input,
						   std::string *errorThrow,
						   uint16_t *port, uint32_t *address);
	bool isKwAllowedInCtx_(State::e_id kw, State::e_id ctx);
	ParserAPI *config_;
	Engine *parser_;
};

typedef State::e_id (Parser::StatelessSet::*StateHandler)
	(const StatefulSet &data);

struct s_trans {
	State::e_id state;
	t_token_type evt;
	StateHandler apply;
	std::string errormess;
};

class Engine: public Analyser {
 public:
	Engine(const std::list<Token> &token, ParserAPI *config);
	State::e_id ParserMainLoop(void);
	void PushContext(const State::e_id &ctx);
	void PopContext(void);
	t_token_type SkipEvent(void);
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

class StatefulSet : public Analyser {
 public:
	StatefulSet(size_t line,
				State::e_id st,
				const std::string &rawData,
				State::e_id ctx,
				const std::string &error,
				size_t argNumber);
	State::e_id GetCtx(void) const;
	State::e_id GetState(void) const;
	const std::string &GetRawData(void) const;
	const std::string &GetErrorMessage(void) const;
	size_t GetLineNumber(void) const;
	size_t GetArgNumber(void) const;

 private:
	const std::string &error_msg_;
	const State::e_id state_;
	const std::string &rawData_;
	const State::e_id ctx_;
	const size_t argNumber_;
};

}  // namespace Parser

typedef enum Parser::State::e_id t_parsing_state;

#endif  // SRCS_INCS_PARSER_PARSER_HPP_
