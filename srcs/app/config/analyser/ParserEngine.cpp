#include <parser/ParserEngine.hpp>

Parser::Engine::Engine(const std::list<Token> &token, Wrapper *config) :
	handlers_(this, config),
	ite_(token.end()),
	itc_(token.begin()),
	transitions_(TransitionFactory_()),
	args_() {
	line_ = 1;
	ctx_.push(Parser::State::K_INIT);
	ParserMainLoop();
}

t_evt Parser::Event::GetEventTypeEnum(const Token &token) {
	return static_cast<t_evt>(token.GetType());
}

void Parser::Engine::PushContext(const t_parsing_state &ctx) {
	ctx_.push(ctx);
}

void Parser::Engine::PopContext(void) {
	ctx_.pop();
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
			 i < transitions_.size();
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


std::vector < Parser::s_trans > Parser::Engine::TransitionFactory_(void) {
	std::vector < Parser::s_trans > ret;
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INIT,
			.evt = Parser::Event::EVT_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::InitHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INIT,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `{'"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Parser::Event::EVT_SCOPE_CLOSE,
			.apply = &Parser::StatelessSet::ExpKwHandlerClose,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ExpKwHandlerKw,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_KW,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting keyword"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_SEMIC,
			.evt = Parser::Event::EVT_SEMICOLON,
			.apply = &Parser::StatelessSet::SemicHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_EXP_SEMIC,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `;'"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_AUTOINDEX,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::AutoindexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_AUTOINDEX,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `on' or `off' after `autoindex'"
			" directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ServerNameHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Parser::Event::EVT_SEMICOLON,
			.apply = &Parser::StatelessSet::ServerNameHandlerSemic,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER_NAME,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `server_name' "
			"directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LOCATION,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::LocationHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LOCATION,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting path after `location' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER,
			.evt = Parser::Event::EVT_SCOPE_OPEN,
			.apply = &Parser::StatelessSet::ServerHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_SERVER,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Expecting `{' after `server' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LISTEN,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ListenHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LISTEN,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `listen' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ERROR_PAGE,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ErrorPageHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ERROR_PAGE,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `error_page' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_CGI_ASSIGN,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::CgiAssignHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_CGI_ASSIGN,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `cgi_assign' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ROOT,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::RootHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_ROOT,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `root' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INDEX,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::IndexHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_INDEX,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `index' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::LimitExceptHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Parser::Event::EVT_SEMICOLON,
			.apply = &Parser::StatelessSet::LimitExceptHandlerSemic,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_LIMIT_EXCEPT,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `limit_except' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_RETURN,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ReturnHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_RETURN,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `return' directive"});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_UPLOAD_STORE,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::UploadStoreHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state = Parser::State::K_UPLOAD_STORE,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `upload_store' directive"});
	ret.push_back((Parser::s_trans){.state =
			Parser::State::K_CLIENT_MAX_BODY_SIZE,
			.evt = Parser::Event::EVT_WORD,
			.apply = &Parser::StatelessSet::ClientMaxBodySizeHandler,
			.errormess = ""});
	ret.push_back((Parser::s_trans){.state =
			Parser::State::K_CLIENT_MAX_BODY_SIZE,
			.evt = Parser::Event::EVT_NONE,
			.apply = &Parser::StatelessSet::SyntaxFailer,
			.errormess = "Unexpected token after `client_max_body_size'"
			" directive"});
	return ret;
}
