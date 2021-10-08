#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

Parser::Parser(const std::list<Token> &token, ParserAPI *config) :
	handlers_(this),
	config_(config),
	itb_(token.begin()),
	ite_(token.end()),
	itc_(itb_),
	argNumber_(0) {
	ctx_.push(Token::State::K_INIT);
	parse();
}

// Data::Data(Parser * const parser, const std::string &error_msg) :
// 	error_msg_(error_msg),
// 	line_(parser->itc_->GetLine()),
// 	event_(parser->itc_->getType()),
// 	state_(parser->itc_->GetState()),
// 	rawData_(parser->itc_->getRawData()),
// 	ctx_(parser->TopContext_()),
// 	config_(parser->config_) {
// }

Data::Data(size_t line,
				   t_token_type evt,
				   t_parsing_state st,
				   const std::string &rawData,
				   t_parsing_state ctx,
				   ParserAPI *config,
				   const std::string &error) :
	error_msg_(error),
	event_(evt),
	state_(st),
	rawData_(rawData),
	ctx_(ctx),
	config_(config) {
	line_ = line;
}

// probably the context sensitiveness for the setters should be implemented
// in here Parser.hpp/cpp, not in Config.hpp/cpp

void Data::SetListenAddress(const std::string &svnaddr) const {
	const char * addressStr = svnaddr.c_str();
	std::string addTmp;  // we need this objects lifetime to last for the entire
						 // function
	int64_t port = 8080;
	char *endptr = NULL;
	if (std::count(svnaddr.begin(), svnaddr.end(), '.') != 3) {
		port = std::strtol(addressStr, &endptr, 10);
		if (*endptr || errno || port < 1 || port > UINT16_MAX) {
			throw SyntaxError("Listen directive port invalid", line_);
		} else {
			config_->SetListenAddress(0, static_cast<uint16_t>(port), ctx_);
		}
		return;
	}
	if (std::count(svnaddr.begin(), svnaddr.end(), ':') == 1) {
		addTmp = svnaddr.substr(0, svnaddr.find(':'));
		std::string portTmp =
			svnaddr.substr(svnaddr.find(':') + 1, svnaddr.size());
		port = std::strtol(portTmp.c_str(), &endptr, 10);
		if ((endptr && *endptr) || errno || port < 1 || port > UINT16_MAX) {
			throw SyntaxError("Listen directive port invalid", line_);
		}
		addressStr = addTmp.c_str();
	}
	const in_addr_t address = inet_addr(addressStr);
	if (address == static_cast<in_addr_t>(-1)) {
		throw SyntaxError("listen directive IP invalid", line_);
	}
	config_->SetListenAddress(ntohl(address), static_cast<uint16_t>(port),
							ctx_);
}

void Data::AddLocation(const std::string &path) const {
	// path should be in location ctor
	config_->AddLocation(path, ctx_);
}

void Data::AddServerName(const std::string &name) const {
	config_->AddServerName(name, ctx_);
}

void Data::SetRoot(const std::string &root) const {
	config_->SetRoot(root, ctx_);
}

void Data::AddIndex(const std::string &index) const {
	config_->AddIndex(index, ctx_);
}

void Data::AddAutoindex(const std::string &autoindex) const {
	config_->AddAutoindex(autoindex == "on", ctx_);
}

void Data::SetClientMaxSz(uint32_t size) const {
	config_->SetClientMaxSz(size, ctx_);
}

void Data::AddServer(void) const {
	config_->AddServer(ctx_);
}

t_parsing_state Parser::StHandler::InitHandler(const Data &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StHandler::SemicHandler(const Data &data) {
	(void) data;
	return Token::State::K_EXP_KW;
}

t_parsing_state Parser::StHandler::SyntaxFailer(const Data &data) {
	std::cerr << "Raw data: \""<< data.GetRawData() << "\"\n";
	// std::cerr << "Token type: \""<< data.current_.GetTokenTypeStr()
	//		  << "\"\n";
	std::cerr << "Event type: \""<<  data.GetEvent() << "\"\n";
	std::cerr << "State type: \""<<  data.GetState() << "\"\n";
	std::string result = "Syntax error: " + data.GetErrorMessage();
	throw SyntaxError(result, LINE);
}

t_parsing_state Parser::StHandler::ExpKwHandlerClose(const Data &data) {
	(void)data;
	parser_->PopContext_();
	return Token::State::K_EXIT;
}

t_parsing_state Parser::StHandler::ExpKwHandlerKw(const Data &data) {
	if (data.GetState() < Token::State::K_SERVER
	|| data.GetState() > Token::State::K_LIMIT_EXCEPT)
		throw SyntaxError("Expecting keyword but found '" +
		data.GetRawData() + "'", data.GetLineNumber());
	return data.GetState();
}

t_parsing_state Parser::StHandler::AutoindexHandler(const Data &data) {
	if (data.GetRawData() != "on"
	&& data.GetRawData() != "off")
		throw SyntaxError("Expecting 'on'/'off' but found '" +
		data.GetRawData()  + "'", data.GetLineNumber());
	data.AddAutoindex(data.GetRawData());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StHandler::ServerNameHandler(const Data &data) {
	if (parser_->GetArgNumber() == 0
		&& data.GetEvent() == Token::Type::T_SEMICOLON)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"'server_name' directive", LINE);
	if (data.GetEvent() == Token::Type::T_SEMICOLON) {
		parser_->ResetArgNumber();
		return Token::State::K_EXP_KW;
	}
	data.AddServerName(data.GetRawData());
	parser_->IncrementArgNumber();
	return Token::State::K_SERVER_NAME;
}


t_parsing_state Parser::StHandler::LocationHandler(const Data &data) {
	data.AddLocation(data.GetRawData());
	parser_->PushContext_(Token::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StHandler::ListenHandler(const Data &data) {
	data.SetListenAddress(data.GetRawData());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StHandler::ServerHandler(const Data &data) {
	data.AddServer();
	parser_->PushContext_(Token::State::K_SERVER);
	return parser_->ParserMainLoop();
}

void Parser::PushContext_(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

void Parser::PopContext_(void) {
	ctx_.pop();
}

t_token_type Parser::SkipEvent(void) {
	++itc_;
	if (itc_ != ite_)
		return itc_->getType();
	throw SyntaxError("Attempting to read the token past the end of the file"
	, (--itc_)->GetLine());
}

t_token_type Data::GetEvent(void) const {
	return event_;
}

t_parsing_state Data::GetState(void) const {
	return state_;
}

const std::string &Data::GetRawData(void) const {
	return rawData_;
}

const std::string &Data::GetErrorMessage(void) const {
	return error_msg_;
}

size_t Data::GetLineNumber(void) const {
	return line_;
}

t_parsing_state Parser::TopContext_(void) const {
	return ctx_.top();
}

size_t Parser::GetArgNumber(void) {
	return argNumber_;
}

void Parser::IncrementArgNumber(void) {
	argNumber_++;
}

void Parser::ResetArgNumber(void) {
	argNumber_ = 0;
}

Parser::StHandler::StHandler(Parser *parser) :
	parser_(parser)
{}

const struct Parser::s_trans Parser::transitions[18] = {
	{ .state = Token::State::K_INIT,
	  .evt = Token::Type::T_SCOPE_OPEN,
	  .apply = &StHandler::InitHandler,
	  .errormess = ""},
	{ .state = Token::State::K_INIT,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting {"},
	{ .state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_SCOPE_CLOSE,
	  .apply = &StHandler::ExpKwHandlerClose,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::ExpKwHandlerKw,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting keyword"},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_SEMICOLON,
	  .apply = &StHandler::SemicHandler,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting ;"},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::AutoindexHandler,
	  .errormess = ""},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting 'on' or 'off'"},
	{ .state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::ServerNameHandler,
	  .errormess = ""},
	{ .state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_SEMICOLON,
	  .apply = &StHandler::ServerNameHandler,
	  .errormess = ""},
	{ .state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting some server names"},
	{ .state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::LocationHandler,
	  .errormess = ""},
	{ .state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting path after location directive"},
	{ .state = Token::State::K_SERVER,
	  .evt = Token::Type::T_SCOPE_OPEN,
	  .apply = &StHandler::ServerHandler,
	  .errormess = ""},
	{ .state = Token::State::K_SERVER,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting { after server directive"},
	{ .state = Token::State::K_LISTEN,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::ListenHandler,
	  .errormess = ""},
	{ .state = Token::State::K_LISTEN,
	  .evt = Token::Type::T_NONE,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting IP in listen directive"},
};

t_parsing_state Parser::ParserMainLoop(void) {
	t_parsing_state state;
	for (state = Token::State::K_INIT;
			itc_ != ite_ ; itc_++) {
		t_token_type event = itc_->getType();
		for (size_t i = 0;
			 i < sizeof(transitions) / sizeof(transitions[0]);
			 ++i) {
			if ((state == transitions[i].state)
				|| (Token::State::K_NONE == transitions[i].state)) {
				if ((event == transitions[i].evt)
					|| (Token::Type::T_NONE == transitions[i].evt)) {
					Data data(itc_->GetLine(),
						itc_->getType(),
						itc_->GetState(),
						itc_->getRawData(),
						ctx_.top(),
						config_,
						transitions[i].errormess);
					state = ((handlers_).*(transitions[i].apply))(data);
					if (state == Token::State::K_EXIT)
						return Token::State::K_EXP_KW;
					break;
				}
			}
		}
	}
	throw SyntaxError("Unclosed scope", (--itc_)->GetLine());
}

void Parser::parse(void) {
	ParserMainLoop();
}
