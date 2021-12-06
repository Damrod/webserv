#include <parser/ParserEngine.hpp>

Parser::Engine::Engine(const std::list<Token> &token, Wrapper *config) :
	handlers_(this, config),
	ite_(token.end()),
	itc_(token.begin()),
	args_() {
	line_ = 1;
	ctx_.push(Parser::State::K_INIT);
	ParserMainLoop();
}

Parser::Engine::~Engine(void) {
	itc_++;
	if (itc_ != ite_) {
		throw
			SyntaxError("There are unbalanced "
						"closing curly braces at the end of the file",
						(--itc_)->GetLine());
	}
}

t_evt Parser::Event::GetEventTypeEnum(const Token &token) {
	return static_cast<t_evt>(token.GetType());
}

void Parser::Engine::PushContext(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

Parser::State::e_id Parser::Engine::PopContext(void) {
	Parser::State::e_id popped_context = ctx_.top();

	ctx_.pop();
	return popped_context;
}

t_evt Parser::Engine::SkipEvent(void) {
	++itc_;
	if (itc_ != ite_)
		return Event::GetEventTypeEnum(*itc_);
	throw SyntaxError("Attempting to read the token past the end of the file"
					  , (--itc_)->GetLine());
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

t_parsing_state Parser::Engine::ParserMainLoop(void) {
	t_parsing_state state = Parser::State::K_INIT;
	for (; itc_ != ite_ ; itc_++) {
		t_evt event = Event::GetEventTypeEnum(*itc_);
		for (size_t i = 0;
			 i < (sizeof(transitions_) / sizeof(transitions_[0]));
			 ++i) {
			if ((state == transitions_[i].state)
				|| (Parser::State::K_NONE == transitions_[i].state)) {
				if ((event == transitions_[i].evt)
					|| (Parser::Event::EVT_NONE == transitions_[i].evt)) {
					StatefulSet data(itc_->GetLine(),
						Parser::State::GetParsingStateTypeEnum(*itc_),
									 itc_->GetRawData(),
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

const Parser::s_trans Parser::Engine::transitions_[35] = {
	{Parser::State::K_INIT,
		Parser::Event::EVT_SCOPE_OPEN,
		&Parser::StatelessSet::InitHandler,
		""},
	{Parser::State::K_INIT,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting `{'"},
	{Parser::State::K_EXP_KW,
		Parser::Event::EVT_SCOPE_CLOSE,
		&Parser::StatelessSet::ExpKwHandlerClose,
		""},
	{Parser::State::K_EXP_KW,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ExpKwHandlerKw,
		""},
	{Parser::State::K_EXP_KW,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting keyword"},
	{Parser::State::K_EXP_SEMIC,
		Parser::Event::EVT_SEMICOLON,
		&Parser::StatelessSet::SemicHandler,
		""},
	{Parser::State::K_EXP_SEMIC,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting `;'"},
	{Parser::State::K_AUTOINDEX,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::AutoindexHandler,
		""},
	{Parser::State::K_AUTOINDEX,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting `on' or `off' after `autoindex'"
					 " directive"},
	{Parser::State::K_SERVER_NAME,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ServerNameHandler,
		""},
	{Parser::State::K_SERVER_NAME,
		Parser::Event::EVT_SEMICOLON,
		&Parser::StatelessSet::ServerNameHandlerSemic,
		""},
	{Parser::State::K_SERVER_NAME,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `server_name' "
					 "directive"},
	{Parser::State::K_LOCATION,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::LocationHandler,
		""},
	{Parser::State::K_LOCATION,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting path after `location' directive"},
	{Parser::State::K_SERVER,
		Parser::Event::EVT_SCOPE_OPEN,
		&Parser::StatelessSet::ServerHandler,
		""},
	{Parser::State::K_SERVER,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Expecting `{' after `server' directive"},
	{Parser::State::K_LISTEN,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ListenHandler,
		""},
	{Parser::State::K_LISTEN,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `listen' directive"},
	{Parser::State::K_ERROR_PAGE,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ErrorPageHandler,
		""},
	{Parser::State::K_ERROR_PAGE,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `error_page' directive"},
	{Parser::State::K_CGI_ASSIGN,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::CgiAssignHandler,
		""},
	{Parser::State::K_CGI_ASSIGN,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `cgi_assign' directive"},
	{Parser::State::K_ROOT,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::RootHandler,
		""},
	{Parser::State::K_ROOT,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `root' directive"},
	{Parser::State::K_INDEX,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::IndexHandler,
		""},
	{Parser::State::K_INDEX,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `index' directive"},
	{Parser::State::K_LIMIT_EXCEPT,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::LimitExceptHandler,
		""},
	{Parser::State::K_LIMIT_EXCEPT,
		Parser::Event::EVT_SEMICOLON,
		&Parser::StatelessSet::LimitExceptHandlerSemic,
		""},
	{Parser::State::K_LIMIT_EXCEPT,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `limit_except' directive"},
	{Parser::State::K_RETURN,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ReturnHandler,
		""},
	{Parser::State::K_RETURN,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `return' directive"},
	{Parser::State::K_UPLOAD_STORE,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::UploadStoreHandler,
		""},
	{Parser::State::K_UPLOAD_STORE,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `upload_store' directive"},
	{Parser::State::K_CLIENT_MAX_BODY_SIZE,
		Parser::Event::EVT_WORD,
		&Parser::StatelessSet::ClientMaxBodySizeHandler,
		""},
	{Parser::State::K_CLIENT_MAX_BODY_SIZE,
		Parser::Event::EVT_NONE,
		&Parser::StatelessSet::SyntaxFailer,
		"Unexpected token after `client_max_body_size'"
					 " directive"}
};
