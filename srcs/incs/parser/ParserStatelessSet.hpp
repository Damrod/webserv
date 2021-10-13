#ifndef SRCS_INCS_PARSER_PARSERSTATELESSSET_HPP_
#define SRCS_INCS_PARSER_PARSERSTATELESSSET_HPP_

#include <arpa/inet.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <algorithm>
#include <HttpStatusCodes.hpp>
#include <parser/ParserStatefulSet.hpp>
#include <parser/ParsingState.hpp>
#include <parser/ParsingEvent.hpp>
#include <parser/ParserAPI.hpp>
#include <parser/Analyser.hpp>

// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

namespace Parser {

class Engine;

class StatelessSet : public Analyser {
 public:
// ************* Constructors ************
	StatelessSet(Engine *parser, API *config);

//  *********** State handlers ***********
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
	bool isReturnStatusValid_(int64_t status);
	bool areHttpMethodsValid_(const std::vector<std::string> &input,
							  std::string *error_throw);
	API *config_;
	Engine *parser_;
};

typedef State::e_id (Parser::StatelessSet::*StateHandler)
	(const StatefulSet &data);

struct s_trans {
	State::e_id state;
	Event::e_id evt;
	StateHandler apply;
	std::string errormess;
};

}  // namespace Parser

const std::string	valid_http_methods[] = {"GET", "HEAD", "POST", "PUT",
	"DELETE", "CONNECT", "OPTIONS", "TRACE"};
const uint16_t valid_return_status[] = {301, 302, 303, 307, 308};

#include <parser/ParserEngine.hpp>

#endif  // SRCS_INCS_PARSER_PARSERSTATELESSSET_HPP_
