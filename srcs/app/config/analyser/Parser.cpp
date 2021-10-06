#include <parser/Parser.hpp>
// #define DBG
#ifdef DBG
# define LINE __LINE__
#else
# define LINE data.GetLineNumber()
#endif

Parser::Parser(const std::list<Token> &token, ParserAPI *config) :
	tokens_(token),
	config_(config),
	itb_(tokens_.begin()),
	ite_(tokens_.end()),
	itc_(itb_),
	argNumber_(0) {
	ctx_.push(Token::State::K_INIT);
	parse();
}

Parser::Data::Data(Parser * const parser, const std::string &error_msg) :
	error_msg_(error_msg),
	line_(parser->itc_->GetLine()),
	event_(parser->itc_->getType()),
	state_(parser->itc_->GetState()),
	rawData_(parser->itc_->getRawData()),
	parser_(parser),
	ctx_(parser->TopContext_()),
	config_(parser->config_) {
}

// probably the context sensitiveness for the setters should be implemented
// in here Parser.hpp/cpp, not in Config.hpp/cpp

void Parser::Data::SetListenAddress(uint32_t address, uint16_t port) const {
	config_->SetListenAddress(address, port, ctx_);
}

void Parser::Data::AddLocation(const std::string &path) const {
	// path should be in location ctor
	config_->AddLocation(path, ctx_);
}

void Parser::Data::AddServerName(const std::string &name) const {
	config_->AddServerName(name, ctx_);
}

void Parser::Data::SetRoot(const std::string &root) const {
	config_->SetRoot(root, ctx_);
}

void Parser::Data::AddIndex(const std::string &index) const {
	config_->AddIndex(index, ctx_);
}

void Parser::Data::AddAutoindex(const std::string &autoindex) const {
	config_->AddAutoindex(autoindex == "on", ctx_);
}

void Parser::Data::SetClientMaxSz(uint32_t size) const {
	config_->SetClientMaxSz(size, ctx_);
}

void Parser::Data::AddServer(void) const {
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
	data.PopContext();
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
	if (data.GetArgNumber() == 0 && data.GetEvent() == Token::Type::T_SEMICOLON)
		throw Analyser::SyntaxError("invalid number of arguments in "
									"\"server_name\" directive:", LINE);
	if (data.GetEvent() == Token::Type::T_SEMICOLON) {
		data.ResetArgNumber();
		return Token::State::K_EXP_KW;
	}
	data.AddServerName(data.GetRawData());
	data.IncrementArgNumber();
	return Token::State::K_SERVER_NAME;
}


t_parsing_state Parser::StHandler::LocationHandler(const Data &data) {
	data.AddLocation(data.GetRawData());
	data.PushContext(Token::State::K_LOCATION);
	data.NextEvent();
	return data.ParserLoopBack();
}

t_parsing_state Parser::StHandler::ServerHandler(const Data &data) {
	data.AddServer();
	data.PushContext(Token::State::K_SERVER);
	return data.ParserLoopBack();
}

void Parser::PushContext_(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

void Parser::Data::PopContext(void) const {
	parser_->PopContext_();
}

void Parser::Data::PushContext(const t_parsing_state &ctx) const {
	parser_->PushContext_(ctx);
}

void Parser::PopContext_(void) {
	ctx_.pop();
}

void Parser::Data::NextEvent(void) const {
	parser_->NextEvent();
}


t_token_type Parser::NextEvent(void) {
	++itc_;
	if (itc_ != ite_)
		return itc_->getType();
	return
		Token::Type::T_INVALID;
}

t_parsing_state Parser::Data::ParserLoopBack(void) const {
	return (*parser_).ParserMainLoop();
}

t_token_type Parser::Data::GetEvent(void) const {
	return event_;
}

t_parsing_state Parser::Data::GetState(void) const {
	return state_;
}

const std::string &Parser::Data::GetRawData(void) const {
	return rawData_;
}

const std::string &Parser::Data::GetErrorMessage(void) const {
	return error_msg_;
}

size_t Parser::Data::GetLineNumber(void) const {
	return line_;
}

t_parsing_state Parser::TopContext_(void) const {
	return ctx_.top();
}

void Parser::Data::IncrementArgNumber(void) const {
	parser_->IncrementArgNumber();
}

void Parser::Data::ResetArgNumber(void) const {
	parser_->ResetArgNumber();
}

size_t Parser::Data::GetArgNumber(void) const {
	return parser_->GetArgNumber();
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
const struct Parser::s_trans Parser::transitions[13] = {
	{ .state = Token::State::K_INIT,
	  .evt = Token::Type::T_SCOPE_OPEN,
	  .apply = &StHandler::InitHandler,
	  .errormess = ""},
	{ .state = Token::State::K_INIT,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "expected { in line"},
	{ .state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_SCOPE_CLOSE,
	  .apply = &StHandler::ExpKwHandlerClose,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_KW,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::ExpKwHandlerKw,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_SEMICOLON,
	  .apply = &StHandler::SemicHandler,
	  .errormess = ""},
	{ .state = Token::State::K_EXP_SEMIC,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "expected ; in line "},
	{ .state = Token::State::K_AUTOINDEX,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::AutoindexHandler,
	  .errormess = ""},
	{ .state = Token::State::K_SERVER_NAME,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::ServerNameHandler,
	  .errormess = ""},
	{ .state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::LocationHandler,
	  .errormess = ""},
	{ .state = Token::State::K_LOCATION,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting path after location directive"},
	{ .state = Token::State::K_SERVER,
	  .evt = Token::Type::T_SCOPE_OPEN,
	  .apply = &StHandler::ServerHandler,
	  .errormess = ""},
	{ .state = Token::State::K_SERVER,
	  .evt = Token::Type::T_WORD,
	  .apply = &StHandler::SyntaxFailer,
	  .errormess = "Expecting { after server directive"}
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
					|| (Token::Type::T_WORD == transitions[i].evt)) {
					Data data(this, transitions[i].errormess);
					state = ((handlers_).*(transitions[i].apply))(data);
					if (state == Token::State::K_EXIT)
						return Token::State::K_EXP_KW;
					break;
				}
			}
		}
	}
	throw SyntaxError("Unclosed scope in line", (--itc_)->GetLine());
}

void Parser::parse(void) {
	ParserMainLoop();
}
