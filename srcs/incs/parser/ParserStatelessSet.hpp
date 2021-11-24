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
#include <parser/ParserWrapper.hpp>
#include <parser/Analyser.hpp>
#include <CommonDefinitions.hpp>

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
	StatelessSet(Engine *parser, Wrapper *config);

//  *********** State handlers ***********
	State::e_id SyntaxFailer(const StatefulSet &data) const;
	State::e_id ServerNameHandler(const StatefulSet &data) const;
	State::e_id ServerNameHandlerSemic(const StatefulSet &data) const;
	State::e_id InitHandler(const StatefulSet &data) const;
	State::e_id SemicHandler(const StatefulSet &data) const;
	State::e_id ExpKwHandlerClose(const StatefulSet &data) const;
	State::e_id ExpKwHandlerKw(const StatefulSet &data) const;
	State::e_id AutoindexHandler(const StatefulSet &data) const;
	State::e_id LocationHandler(const StatefulSet &data) const;
	State::e_id ServerHandler(const StatefulSet &data) const;
	State::e_id ListenHandler(const StatefulSet &data) const;
	State::e_id ErrorPageHandler(const StatefulSet &data) const;
	State::e_id CgiAssignHandler(const StatefulSet &data) const;
	State::e_id RootHandler(const StatefulSet &data) const;
	State::e_id IndexHandler(const StatefulSet &data) const;
	State::e_id ClientMaxBodySizeHandler(const StatefulSet &data) const;
	State::e_id ReturnHandler(const StatefulSet &data) const;
	State::e_id LimitExceptHandler(const StatefulSet &data) const;
	State::e_id LimitExceptHandlerSemic(const StatefulSet &data) const;
	State::e_id UploadStoreHandler(const StatefulSet &data) const;

 private:
	// helpers
	bool ParserErrorPage_(const std::vector<std::string> &input,
						 uint16_t *code, std::string *uri) const;
	bool ParseIpAddressPort_(const std::string &input,
						   std::string *errorThrow,
						   uint16_t *port, uint32_t *address) const;
	bool IsKwAllowedInCtx_(State::e_id kw, State::e_id ctx) const;
	bool AreHttpMethodsValid_(const std::vector<std::string> &input,
							  std::string *error_throw) const;
	Wrapper * const config_;
	Engine * const parser_;
};

typedef State::e_id (Parser::StatelessSet::*StateHandler)
	(const StatefulSet &data) const;

struct s_trans {
	State::e_id state;
	Event::e_id evt;
	StateHandler apply;
	char errormess[60];
};

}  // namespace Parser

#include <parser/ParserEngine.hpp>

#endif  // SRCS_INCS_PARSER_PARSERSTATELESSSET_HPP_
