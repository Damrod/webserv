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
	ctx_.push(Parser::State::K_INIT);
	ParserMainLoop();
}

std::vector < Parser::s_trans > Parser::Engine::TransitionFactory_(void) {
	std::vector < Parser::s_trans > ret;
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INIT,
			.evt = Token::Type::T_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::InitHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INIT,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `{'"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Token::Type::T_SCOPE_CLOSE,
			.apply = &Parser::StatelessSet::ExpKwHandlerClose,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ExpKwHandlerKw,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting keyword"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_SEMIC,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::SemicHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_SEMIC,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `;'"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_AUTOINDEX,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::AutoindexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_AUTOINDEX,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `on' or `off' after `autoindex'"
			" directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ServerNameHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::ServerNameHandlerSemic,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `server_name' "
			"directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LOCATION,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::LocationHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LOCATION,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting path after `location' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER,
			.evt = Token::Type::T_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::ServerHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `{' after `server' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LISTEN,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ListenHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LISTEN,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `listen' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ERROR_PAGE,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ErrorPageHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ERROR_PAGE,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `error_page' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_CGI_ASSIGN,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::CgiAssignHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_CGI_ASSIGN,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `cgi_assign' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ROOT,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::RootHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ROOT,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `root' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INDEX,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::IndexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INDEX,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `index' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::LimitExceptHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Token::Type::T_SEMICOLON,
			.apply = &Parser::StatelessSet::LimitExceptHandlerSemic,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `limit_except' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_RETURN,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ReturnHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_RETURN,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `return' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_UPLOAD_STORE,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::UploadStoreHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_UPLOAD_STORE,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `upload_store' directive"});
	ret.push_back((Parser::s_trans){.state =
			Parser::State::K_CLIENT_MAX_BODY_SIZE,
			.evt = Token::Type::T_WORD,
			.apply = &Parser::StatelessSet::ClientMaxBodySizeHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state =
			Parser::State::K_CLIENT_MAX_BODY_SIZE,
			.evt = Token::Type::T_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `client_max_body_size'"
			" directive"});
	return ret;
}

Parser::StatefulSet::StatefulSet(size_t line,
				   t_parsing_state st,
				   const std::string &rawData,
				   t_parsing_state ctx,
				   const std::string &error,
				   size_t argNumber) :
	error_msg_(error),
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
			*errorThrow = "`listen' directive port number invalid";
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
			*errorThrow = "`listen' directive port number invalid";
			return EXIT_FAILURE;
		}
		addressStr = addTmp.c_str();
	}
	const in_addr_t address = inet_addr(addressStr);
	if (address == static_cast<in_addr_t>(-1)) {
		*errorThrow = "`listen' directive IP invalid";
		return EXIT_FAILURE;
	}
	*outAddress = ntohl(address);
	*outPort = static_cast<uint16_t>(port);
	return EXIT_SUCCESS;
}

t_parsing_state Parser::StatelessSet::InitHandler(const StatefulSet &data) {
	(void) data;
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SemicHandler(const StatefulSet &data) {
	(void) data;
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SyntaxFailer(const StatefulSet &data) {
	std::stringstream str;
#ifdef DBG
	str << "Raw data: \""<< data.GetRawData() << "\"\n";
	str << "State type: \""<<  data.GetState() << "\"\n";
#endif
	str << data.GetErrorMessage();
	throw SyntaxError(str.str(), LINE);
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerClose
													(const StatefulSet &data) {
	(void)data;
	parser_->PopContext();
	return Parser::State::K_EXIT;
}

bool Parser::StatelessSet::isKwAllowedInCtx_(t_parsing_state kw,
										t_parsing_state ctx) {
	if ((ctx != Parser::State::K_LOCATION && ctx != Parser::State::K_SERVER
		&& ctx != Parser::State::K_INIT)
	|| (ctx != Parser::State::K_INIT && kw == Parser::State::K_SERVER)
	|| (ctx == Parser::State::K_SERVER && kw == Parser::State::K_LIMIT_EXCEPT)
	|| (ctx == Parser::State::K_LOCATION && (kw == Parser::State::K_LISTEN
									|| kw == Parser::State::K_SERVER_NAME)))
		return false;
	return true;
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerKw(const StatefulSet &data) {
	if (data.GetState() < Parser::State::K_SERVER
	|| data.GetState() > Parser::State::K_LIMIT_EXCEPT)
		throw SyntaxError("Expecting keyword but found `" +
		data.GetRawData() + "'", LINE);
	if (!isKwAllowedInCtx_(data.GetState(), data.GetCtx()))
		throw SyntaxError("Keyword `" + data.GetRawData() + "' "
						  "not allowed in context `" +
						  Parser::State::GetParsingStateTypeStr(data.GetCtx())
						  + "'", LINE);
	return data.GetState();
}

t_parsing_state Parser::StatelessSet::AutoindexHandler
													(const StatefulSet &data) {
	if (data.GetRawData() != "on" && data.GetRawData() != "off")
		throw SyntaxError("Expecting `on' or `off' but found `" +
		data.GetRawData()  + "'", LINE);
	config_->AddAutoindex(data.GetRawData() == "on", data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerNameHandlerSemic
(const StatefulSet &data) {
	if (data.GetArgNumber() == 0)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`server_name' directive", LINE);
	config_->AddServerName(parser_->GetArgs(), data.GetCtx(), LINE);
	parser_->ResetArgNumber();
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::ServerNameHandler
													(const StatefulSet &data) {
	parser_->IncrementArgNumber(data.GetRawData());
	return Parser::State::K_SERVER_NAME;
}

static const char valid_http_methods[3][7] = {"GET", "POST", "DELETE"};

t_parsing_state Parser::StatelessSet::LimitExceptHandlerSemic
(const StatefulSet &data) {
	if (data.GetArgNumber() == 0)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`limit_except' directive", LINE);
	for (unsigned int i = 0; i < parser_->GetArgs().size(); ++i) {
		bool notValidMethod = true;
		for (unsigned j = 0;
			 j < sizeof(valid_http_methods)/sizeof(valid_http_methods[0]);
			 ++j )
			notValidMethod = notValidMethod
				&& (valid_http_methods[j] != parser_->GetArgs()[i]);
		if (notValidMethod)
			throw Analyser::SyntaxError("`" + parser_->GetArgs()[i] + "' is not"
				" a valid http method for `limit_except' directive", LINE);
	}
	config_->AddLimitExcept(parser_->GetArgs(), data.GetCtx(), LINE);
	parser_->ResetArgNumber();
	return Parser::State::K_EXP_KW;
}


t_parsing_state Parser::StatelessSet::LimitExceptHandler
(const StatefulSet &data) {
	parser_->IncrementArgNumber(data.GetRawData());
	return Parser::State::K_LIMIT_EXCEPT;
}

t_parsing_state Parser::StatelessSet::CgiAssignHandler
													(const StatefulSet &data) {
	if (data.GetArgNumber() == 0) {
		parser_->IncrementArgNumber(data.GetRawData());
		return Parser::State::K_CGI_ASSIGN;
	} else if (data.GetArgNumber() == 1) {
		config_->AddCgiAssign(parser_->GetArgs()[0],
							  data.GetRawData(),
							  data.GetCtx(),
							  LINE);
		parser_->ResetArgNumber();
		return Parser::State::K_EXP_SEMIC;
	} else {
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`cgi_assign' directive", LINE);
	}
}

t_parsing_state Parser::StatelessSet::ReturnHandler
(const StatefulSet &data) {
	if (data.GetArgNumber() == 0) {
		parser_->IncrementArgNumber(data.GetRawData());
		return Parser::State::K_RETURN;
	} else if (data.GetArgNumber() == 1) {
		char *endptr = NULL;
		int64_t status = std::strtol(parser_->GetArgs()[0].c_str(),
									 &endptr,
									 10);
		if ((endptr && *endptr) || errno || status < 0
			|| UINT16_MAX < status)
			throw Analyser::SyntaxError("Bad `return' status", LINE);
		config_->AddReturn(static_cast<uint16_t>(status),
						   data.GetRawData(),
						   data.GetCtx(), LINE);
		parser_->ResetArgNumber();
		return Parser::State::K_EXP_SEMIC;
	} else {
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`return' directive", LINE);
	}
}

t_parsing_state Parser::StatelessSet::ErrorPageHandler
													(const StatefulSet &data) {
	if (data.GetArgNumber() == 0) {
		parser_->IncrementArgNumber(data.GetRawData());
		return Parser::State::K_ERROR_PAGE;
	} else if (data.GetArgNumber() == 1) {
		uint16_t code;
		std::string uri;
		parser_->IncrementArgNumber(data.GetRawData());
		if (ParserErrorPage_(parser_->GetArgs(), &code, &uri))
			throw SyntaxError("Failed to parse `error_page' directive "
							  "arguments", LINE);
		config_->AddErrorPage(code, uri, data.GetCtx(), LINE);
		parser_->ResetArgNumber();
		return Parser::State::K_EXP_SEMIC;
	} else {
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`error_page' directive", LINE);
	}
}

t_parsing_state Parser::StatelessSet::ClientMaxBodySizeHandler(
	const StatefulSet &data) {
	char *endptr = NULL;
	int64_t result = std::strtol(data.GetRawData().c_str(), &endptr, 10);
	if ((endptr && *endptr) || result < 0 || UINT32_MAX < result || errno)
		throw SyntaxError("Invalid `client_max_body_size' directive "
						  "argument" , LINE);
	config_->SetClientMaxSz(static_cast<uint32_t>(result), data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::UploadStoreHandler(
	const StatefulSet &data) {
	config_->AddUploadStore(data.GetRawData(), data.GetCtx(),
							LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::RootHandler(const StatefulSet &data) {
	config_->SetRoot(data.GetRawData(), data.GetCtx(),
					 LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::IndexHandler(const StatefulSet &data) {
	config_->AddIndex(data.GetRawData(), data.GetCtx(),
						 LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::LocationHandler(const StatefulSet &data) {
	config_->AddLocation(data.GetRawData(), data.GetCtx(), LINE);
	parser_->PushContext(Parser::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StatelessSet::ListenHandler(const StatefulSet &data) {
	std::string errorThrow = "";
	uint16_t port;
	uint32_t address;
	if (ParseIpAddressPort_(data.GetRawData(), &errorThrow, &port, &address))
		throw SyntaxError(errorThrow, LINE);
	config_->SetListenAddress(address, port, data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerHandler(const StatefulSet &data) {
	(void)data;
	config_->AddServer(data.GetCtx(), LINE);
	parser_->PushContext(Parser::State::K_SERVER);
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
	t_parsing_state state = Parser::State::K_INIT;
	for (; itc_ != ite_ ; itc_++) {
		t_token_type event = itc_->getType();
		for (size_t i = 0;
			 i < transitions_.size();
			 ++i) {
			if ((state == transitions_[i].state)
				|| (Parser::State::K_NONE == transitions_[i].state)) {
				if ((event == transitions_[i].evt)
					|| (Token::Type::T_NONE == transitions_[i].evt)) {
					StatefulSet data(itc_->GetLine(),
						Parser::State::GetParsingStateTypeEnum(*itc_),
						itc_->getRawData(),
						ctx_.top(),
						transitions_[i].errormess,
						 args_.size());
					state = ((handlers_).*(transitions_[i].apply))(data);
					if (state == Parser::State::K_EXIT)
						return Parser::State::K_EXP_KW;
					break;
				}
			}
		}
	}
	throw SyntaxError("Unclosed scope", (--itc_)->GetLine());
}
