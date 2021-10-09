#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

Parser::Engine::Engine(const std::list<Token> &token, ParserAPI *config) :
	handlers_(this, config),
	ite_(token.end()),
	itc_(token.begin()),
	transitions_(TransitionFactory_()),
	args_() {
	line_ = 1;
	ctx_.push(Token::State::K_INIT);
	ParserMainLoop();
}

std::vector < Parser::s_trans > Parser::Engine::TransitionFactory_(void) {
	std::vector < Parser::s_trans > ret;
	ret.push_back((Parser::s_trans){.state = Token::State::K_INIT,
			.evt = Token::Type::T_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::InitHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_INIT,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `{'"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
			.evt = Token::Type::T_SCOPE_CLOSE,
			.apply = &Parser::StatelessSet::ExpKwHandlerClose,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ExpKwHandlerKw,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_KW,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting keyword"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_SEMIC,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::SemicHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_EXP_SEMIC,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting ;"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_AUTOINDEX,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::AutoindexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_AUTOINDEX,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `on' or `off' after autoindex directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ServerNameHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::ServerNameHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER_NAME,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting at least one word after server_name "
			"directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LOCATION,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::LocationHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LOCATION,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting path after location directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER,
			.evt = Token::Type::T_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::ServerHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_SERVER,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting { after server directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LISTEN,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ListenHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_LISTEN,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting IP in listen directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_ERROR_PAGE,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ErrorPageHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_ERROR_PAGE,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::ErrorPageHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_ERROR_PAGE,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after error_page directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_CGI_ASSIGN,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::CgiAssignHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_CGI_ASSIGN,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::CgiAssignHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_CGI_ASSIGN,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after cgi_assign directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_ROOT,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::RootHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_ROOT,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::RootHandler,
			.errormess = "Unexpected token after root directive"});
	ret.push_back((Parser::s_trans){.state = Token::State::K_INDEX,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::IndexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Token::State::K_INDEX,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::IndexHandler,
			.errormess = "Unexpected token after index directive"});
	return ret;
}

Parser::StatefulSet::StatefulSet(size_t line,
				   t_token_type evt,
				   t_parsing_state st,
				   const std::string &rawData,
				   t_parsing_state ctx,
				   const std::string &error,
				   size_t argNumber) :
	error_msg_(error),
	event_(evt),
	state_(st),
	rawData_(rawData),
	ctx_(ctx),
	argNumber_(argNumber) {
	line_ = line;
}

bool Parser::StatelessSet::ParserErrorPage_(
					 const std::vector<std::string> &input,
					 uint16_t *code, std::string *uri) {
	char *endptr = NULL;
	if (input.size() != 2)
		return EXIT_FAILURE;
	*code = std::strtol(input[0].c_str(), &endptr, 10);
	if ((endptr && *endptr) || errno || *code < 100 || *code > 505)
		return EXIT_FAILURE;
	*uri = input[1];
	return EXIT_SUCCESS;
}

bool Parser::StatelessSet::ParseIpAddressPort_(const std::string &input,
										 std::string *errorThrow,
										 uint16_t *outPort,
										 uint32_t *outAddress) {
	const char * addressStr = input.c_str();
	std::string addTmp;  // we need this object's lifetime to last for the
						 // entire function
	int64_t port = 8080;
	char *endptr = NULL;
	if (std::count(input.begin(), input.end(), '.') != 3) {
		port = std::strtol(addressStr, &endptr, 10);
		if (*endptr || errno || port < 1 || port > UINT16_MAX) {
			*errorThrow = "Listen directive port invalid";
			return EXIT_FAILURE;
		}
		*outAddress = 0;
		*outPort = static_cast<uint16_t>(port);
		return EXIT_SUCCESS;
	}
	if (std::count(input.begin(), input.end(), ':') == 1) {
		addTmp = input.substr(0, input.find(':'));
		std::string portTmp =
			input.substr(input.find(':') + 1, input.size());
		port = std::strtol(portTmp.c_str(), &endptr, 10);
		if ((endptr && *endptr) || errno || port < 1 || port > UINT16_MAX) {
			*errorThrow = "Listen directive port invalid";
			return EXIT_FAILURE;
		}
		addressStr = addTmp.c_str();
	}
	const in_addr_t address = inet_addr(addressStr);
	if (address == static_cast<in_addr_t>(-1)) {
		*errorThrow = "listen directive IP invalid";
		return EXIT_FAILURE;
	}
	*outAddress = ntohl(address);
	*outPort = static_cast<uint16_t>(port);
	return EXIT_SUCCESS;
}

t_parsing_state Parser::StatelessSet::InitHandler(const StatefulSet &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SemicHandler(const StatefulSet &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SyntaxFailer(const StatefulSet &data) {
	std::stringstream str;
#ifdef DBG
	str << "Raw data: \""<< data.GetRawData() << "\"\n";
	str << "Event type: \""<<  data.GetEvent() << "\"\n";
	str << "State type: \""<<  data.GetState() << "\"\n";
#endif
	str << data.GetErrorMessage();
	throw SyntaxError(str.str(), LINE);
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerClose
													(const StatefulSet &data) {
	(void)data;
	parser_->PopContext();
	return Token::State::K_EXIT;
}

bool Parser::StatelessSet::isKwAllowedInCtx_(t_parsing_state kw,
										t_parsing_state ctx) {
	if ((ctx != Token::State::K_LOCATION && ctx != Token::State::K_SERVER
		&& ctx != Token::State::K_INIT)
	|| (ctx == Token::State::K_INIT && kw != Token::State::K_SERVER)
	|| (ctx == Token::State::K_SERVER && kw == Token::State::K_LIMIT_EXCEPT)
	|| (ctx == Token::State::K_LOCATION && (kw == Token::State::K_LISTEN
									|| kw == Token::State::K_SERVER_NAME)))
		return false;
	return true;
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerKw(const StatefulSet &data) {
	if (data.GetState() < Token::State::K_SERVER
	|| data.GetState() > Token::State::K_LIMIT_EXCEPT)
		throw SyntaxError("Expecting keyword but found `" +
		data.GetRawData() + "'", data.GetLineNumber());
	if (!isKwAllowedInCtx_(data.GetState(), data.GetCtx()))
		throw SyntaxError("Keyword `" + data.GetRawData() + "' "
						  "not allowed in context `" +
						  Token::State::GetParsingStateTypeStr(data.GetCtx())
						  + "'", data.GetLineNumber());
	return data.GetState();
}

t_parsing_state Parser::StatelessSet::AutoindexHandler
													(const StatefulSet &data) {
	if (data.GetRawData() != "on"
	&& data.GetRawData() != "off")
		throw SyntaxError("Expecting `on'/`off' but found `" +
		data.GetRawData()  + "'", data.GetLineNumber());
	config_->AddAutoindex(data.GetRawData() == "on", data.GetCtx(),
						  data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerNameHandler
													(const StatefulSet &data) {
	if (data.GetArgNumber() == 0
		&& data.GetEvent() == Token::Type::T_SEMICOLON)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`server_name' directive", LINE);
	if (data.GetEvent() == Token::Type::T_SEMICOLON) {
		config_->AddServerName(parser_->GetArgs(), data.GetCtx(),
							   data.GetLineNumber());
		parser_->ResetArgNumber();
		return Token::State::K_EXP_KW;
	}
	parser_->IncrementArgNumber(data.GetRawData());
	return Token::State::K_SERVER_NAME;
}

t_parsing_state Parser::StatelessSet::CgiAssignHandler
													(const StatefulSet &data) {
	switch (data.GetArgNumber()) {
	case 0:
	case 1: {
		switch (data.GetEvent()) {
		case Token::Type::T_WORD: {
			parser_->IncrementArgNumber(data.GetRawData());
			return Token::State::K_CGI_ASSIGN;
		}
		default: {
			throw Analyser::SyntaxError("Unexpected token after "
										"cgi_assign directive", LINE);
		}
		}
	}
	case 2: {
		switch (data.GetEvent()) {
		case Token::Type::T_SEMICOLON: {
			config_->AddCgiAssign(parser_->GetArgs()[0],
								  parser_->GetArgs()[1],
								  data.GetCtx(),
								  data.GetLineNumber());
			parser_->ResetArgNumber();
			return Token::State::K_EXP_KW;
		}
		default: {
			throw Analyser::SyntaxError("Invalid number of arguments in "
										"cgi_assign directive", LINE);
		}
		}
	}
	default : {
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"cgi_assign directive", LINE);
	}
	}
}

t_parsing_state Parser::StatelessSet::ErrorPageHandler
													(const StatefulSet &data) {
	switch (data.GetArgNumber()) {
	case 0:
	case 1: {
		switch (data.GetEvent()) {
		case Token::Type::T_WORD: {
			parser_->IncrementArgNumber(data.GetRawData());
			return Token::State::K_ERROR_PAGE;
		}
		default: {
			throw Analyser::SyntaxError("Unexpected token after "
										"error_page directive", LINE);
		}
		}
	}
	case 2: {
		switch (data.GetEvent()) {
		case Token::Type::T_SEMICOLON: {
			uint16_t code;
			std::string uri;
			if (ParserErrorPage_(parser_->GetArgs(), &code, &uri))
				throw SyntaxError("Failed to parse error_page directive "
								  "arguments", data.GetLineNumber());
			config_->AddErrorPage(code, uri, data.GetCtx(),
								  data.GetLineNumber());
			parser_->ResetArgNumber();
			return Token::State::K_EXP_KW;
		}
		default: {
			throw Analyser::SyntaxError("Invalid number of arguments in "
										"error_page directive", LINE);
		}
		}
	}
	default : {
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"error_page directive", LINE);
	}
	}
}

t_parsing_state Parser::StatelessSet::RootHandler(const StatefulSet &data) {
	config_->SetRoot(data.GetRawData(), data.GetCtx(),
					 data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::IndexHandler(const StatefulSet &data) {
	config_->AddIndex(data.GetRawData(), data.GetCtx(),
						 data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::LocationHandler(const StatefulSet &data) {
	config_->AddLocation(data.GetRawData(), data.GetCtx(),
						 data.GetLineNumber());
	parser_->PushContext(Token::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StatelessSet::ListenHandler(const StatefulSet &data) {
	std::string errorThrow;
	uint16_t port;
	uint32_t address;
	if (ParseIpAddressPort_(data.GetRawData(), &errorThrow,
											&port, &address))
		throw SyntaxError(errorThrow, line_);
	config_->SetListenAddress(address, port, data.GetCtx(),
							  data.GetLineNumber());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerHandler(const StatefulSet &data) {
	(void)data;
	config_->AddServer(data.GetCtx(), data.GetLineNumber());
	parser_->PushContext(Token::State::K_SERVER);
	return parser_->ParserMainLoop();
}

void Parser::Engine::PushContext(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

void Parser::Engine::PopContext(void) {
	ctx_.pop();
}

t_token_type Parser::Engine::SkipEvent(void) {
	++itc_;
	if (itc_ != ite_)
		return itc_->getType();
	throw SyntaxError("Attempting to read the token past the end of the file"
	, (--itc_)->GetLine());
}

t_token_type Parser::StatefulSet::GetEvent(void) const {
	return event_;
}

t_parsing_state Parser::StatefulSet::GetCtx(void) const {
	return ctx_;
}

t_parsing_state Parser::StatefulSet::GetState(void) const {
	return state_;
}

const std::string &Parser::StatefulSet::GetRawData(void) const {
	return rawData_;
}

const std::string &Parser::StatefulSet::GetErrorMessage(void) const {
	return error_msg_;
}

size_t Parser::StatefulSet::GetLineNumber(void) const {
	return line_;
}

size_t Parser::StatefulSet::GetArgNumber(void) const {
	return argNumber_;
}

void Parser::Engine::IncrementArgNumber(const std::string &arg) {
	args_.push_back(arg);
}

const std::vector<std::string> &Parser::Engine::GetArgs(void) const {
	return args_;
}

void Parser::Engine::ResetArgNumber(void) {
	args_.clear();
}

Parser::StatelessSet::StatelessSet(Engine *parser, ParserAPI *config) :
	config_(config),
	parser_(parser) {}

t_parsing_state Parser::Engine::ParserMainLoop(void) {
	t_parsing_state state = Token::State::K_INIT;
	for (; itc_ != ite_ ; itc_++) {
		t_token_type event = itc_->getType();
		for (size_t i = 0;
			 i < transitions_.size();
			 ++i) {
			if ((state == transitions_[i].state)
				|| (Token::State::K_NONE == transitions_[i].state)) {
				if ((event == transitions_[i].evt)
					|| (Token::Type::T_NONE == transitions_[i].evt)) {
					StatefulSet data(itc_->GetLine(),
						itc_->getType(),
						itc_->GetState(),
						itc_->getRawData(),
						ctx_.top(),
						transitions_[i].errormess,
						 args_.size());
					state = ((handlers_).*(transitions_[i].apply))(data);
					if (state == Token::State::K_EXIT)
						return Token::State::K_EXP_KW;
					break;
				}
			}
		}
	}
	throw SyntaxError("Unclosed scope", (--itc_)->GetLine());
}
