#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

Parser::Engine::Engine(const std::list<Token> &token, ParserAPI *config) :
	handlers_(this, config),
	config_(config),
	ite_(token.end()),
	itc_(token.begin()),
	transitions_(TransitionFactory_()),
	argNumber_(0) {
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
	  .errormess = "Expecting {"});
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
	  .errormess = "Expecting 'on' or 'off'"});
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
	  .errormess = "Expecting some server names"});
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
	return ret;
}

//  const std::vector < Parser::s_trans > Parser::Engine::transitions =
//												  TransitionFactory_();

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

void Parser::StatelessSet::SetListenAddress(const std::string &svnaddr,
											t_parsing_state ctx) const {
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
			config_->SetListenAddress(0, static_cast<uint16_t>(port), ctx);
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
							ctx);
}

void Parser::StatelessSet::AddLocation(const std::string &path,
									   t_parsing_state ctx) const {
	// path should be in location ctor
	config_->AddLocation(path, ctx);
}

void Parser::StatelessSet::AddServerName(const std::string &name,
										 t_parsing_state ctx) const {
	config_->AddServerName(name, ctx);
}

void Parser::StatelessSet::SetRoot(const std::string &root,
								   t_parsing_state ctx) const {
	config_->SetRoot(root, ctx);
}

void Parser::StatelessSet::AddIndex(const std::string &index,
									t_parsing_state ctx) const {
	config_->AddIndex(index, ctx);
}

void Parser::StatelessSet::AddAutoindex(const std::string &autoindex,
										t_parsing_state ctx) const {
	config_->AddAutoindex(autoindex == "on", ctx);
}

void Parser::StatelessSet::SetClientMaxSz(uint32_t size,
										  t_parsing_state ctx) const {
	config_->SetClientMaxSz(size, ctx);
}

void Parser::StatelessSet::AddServer(t_parsing_state ctx) const {
	config_->AddServer(ctx);
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

t_parsing_state Parser::StatelessSet::ExpKwHandlerKw(const StatefulSet &data) {
	if (data.GetState() < Token::State::K_SERVER
	|| data.GetState() > Token::State::K_LIMIT_EXCEPT)
		throw SyntaxError("Expecting keyword but found '" +
		data.GetRawData() + "'", data.GetLineNumber());
	return data.GetState();
}

t_parsing_state Parser::StatelessSet::AutoindexHandler
													(const StatefulSet &data) {
	if (data.GetRawData() != "on"
	&& data.GetRawData() != "off")
		throw SyntaxError("Expecting 'on'/'off' but found '" +
		data.GetRawData()  + "'", data.GetLineNumber());
	AddAutoindex(data.GetRawData(), data.GetCtx());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerNameHandler
													(const StatefulSet &data) {
	if (data.GetArgNumber() == 0
		&& data.GetEvent() == Token::Type::T_SEMICOLON)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"'server_name' directive", LINE);
	if (data.GetEvent() == Token::Type::T_SEMICOLON) {
		parser_->ResetArgNumber();
		return Token::State::K_EXP_KW;
	}
	AddServerName(data.GetRawData(), data.GetCtx());
	parser_->IncrementArgNumber();
	return Token::State::K_SERVER_NAME;
}


t_parsing_state Parser::StatelessSet::LocationHandler(const StatefulSet &data) {
	AddLocation(data.GetRawData(), data.GetCtx());
	parser_->PushContext(Token::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StatelessSet::ListenHandler(const StatefulSet &data) {
	SetListenAddress(data.GetRawData(), data.GetCtx());
	return Token::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerHandler(const StatefulSet &data) {
	(void)data;
	AddServer(data.GetCtx());
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

void Parser::Engine::IncrementArgNumber(void) {
	argNumber_++;
}

void Parser::Engine::ResetArgNumber(void) {
	argNumber_ = 0;
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
						argNumber_);
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
