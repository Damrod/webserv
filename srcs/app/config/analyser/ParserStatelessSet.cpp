#include <parser/ParserStatelessSet.hpp>

Parser::StatelessSet::StatelessSet(Engine *parser, Wrapper *config) :
	config_(config),
	parser_(parser) {}

bool Parser::StatelessSet::ParserErrorPage_(
					 const std::vector<std::string> &input,
					 uint16_t *code, std::string *uri) const {
	char *endptr = NULL;
	if (input.size() != 2)
		return EXIT_FAILURE;
	int64_t result = std::strtol(input[0].c_str(), &endptr, 10);
	if ((endptr && endptr[0] != '\0') || errno ||
		!HttpStatusCodes::IsValid(result))
		return EXIT_FAILURE;
	*code = result;
	*uri = input[1];
	return EXIT_SUCCESS;
}

bool Parser::StatelessSet::ParseIpAddressPort_(const std::string &input,
										 std::string *errorThrow,
										 uint16_t *outPort,
										 uint32_t *outAddress) const {
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

t_parsing_state Parser::StatelessSet::InitHandler(
	const StatefulSet &data) const {
	(void) data;
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SemicHandler(
	const StatefulSet &data) const {
	(void) data;
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::SyntaxFailer(
	const StatefulSet &data) const {
	std::stringstream str;
#ifdef DBG
	str << "Raw data: \""<< data.GetRawData() << "\"\n";
	str << "State type: \""<<  data.GetState() << "\"\n";
#endif
	str << data.GetErrorMessage();
	throw SyntaxError(str.str(), LINE);
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerClose (
	__attribute__((unused)) const StatefulSet &data) const {
	if (parser_->PopContext() == Parser::State::K_SERVER
		&& !ServerHasValidServerNames_() {
		throw SyntaxError("Err");
	}
	return Parser::State::K_EXIT;
}

bool Parser::StatelessSet::IsKwAllowedInCtx_(t_parsing_state kw,
											 t_parsing_state ctx) const {
	if ((ctx != Parser::State::K_LOCATION && ctx != Parser::State::K_SERVER
		&& ctx != Parser::State::K_INIT)
	|| (ctx == Parser::State::K_INIT && kw != Parser::State::K_SERVER)
	|| (ctx == Parser::State::K_SERVER && (kw == Parser::State::K_LIMIT_EXCEPT
	|| kw == Parser::State::K_SERVER))
	|| (ctx == Parser::State::K_LOCATION && (kw == Parser::State::K_LISTEN
									|| kw == Parser::State::K_SERVER_NAME
									|| kw == Parser::State::K_SERVER
									|| kw == Parser::State::K_LOCATION)))
		return false;
	return true;
}

t_parsing_state Parser::StatelessSet::ExpKwHandlerKw(
	const StatefulSet &data) const {
	if (data.GetState() < Parser::State::K_SERVER
	|| data.GetState() > Parser::State::K_LIMIT_EXCEPT) {
		throw SyntaxError("Expecting keyword but found `" +
			data.GetRawData() + "'", LINE);
	}
	if (!IsKwAllowedInCtx_(data.GetState(), data.GetCtx())) {
		std::string state = Parser::State::GetParsingStateTypeStr(data.GetCtx());
		if (!state.empty()) {
			throw SyntaxError("Keyword `" + data.GetRawData() + "' "
					"not allowed in context `" + state + "'", LINE);
		} else {
			throw SyntaxError("Keyword `" + data.GetRawData() + "' "
					"not allowed in global scope", LINE);
		}
	}
	return data.GetState();
}

t_parsing_state Parser::StatelessSet::AutoindexHandler
													(const StatefulSet &data) const {
	if (data.GetRawData() != "on" && data.GetRawData() != "off") {
		throw SyntaxError("Expecting `on' or `off' but found `" +
			data.GetRawData() + "'", LINE);
	}
	config_->SetAutoindex(data.GetRawData() == "on", data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerNameHandlerSemic
(const StatefulSet &data) const {
	if (data.GetArgNumber() == 0)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`server_name' directive", LINE);
	config_->AddServerName(parser_->GetArgs(), data.GetCtx(), LINE);
	parser_->ResetArgNumber();
	return Parser::State::K_EXP_KW;
}

t_parsing_state Parser::StatelessSet::ServerNameHandler
													(const StatefulSet &data) const {
	parser_->IncrementArgNumber(data.GetRawData());
	return Parser::State::K_SERVER_NAME;
}

bool Parser::StatelessSet::AreHttpMethodsValid_(const std::vector<std::string>
										&input, std::string *error_throw) const {
	for (unsigned int i = 0; i < input.size(); ++i) {
		if (!Constants::IsValidMethod(input[i])) {
			*error_throw = "`" + input[i] + "' is not"
				" a valid http method for `limit_except' directive";
			return false;
		}
	}
	return true;
}


t_parsing_state Parser::StatelessSet::LimitExceptHandlerSemic
(const StatefulSet &data) const {
	std::string error_throw;
	if (data.GetArgNumber() == 0)
		throw Analyser::SyntaxError("Invalid number of arguments in "
									"`limit_except' directive", LINE);
	if (!AreHttpMethodsValid_(parser_->GetArgs(), &error_throw)) {
		throw Analyser::SyntaxError(error_throw, LINE);
	}
	config_->SetLimitExcept(parser_->GetArgs(), data.GetCtx(), LINE);
	parser_->ResetArgNumber();
	return Parser::State::K_EXP_KW;
}


t_parsing_state Parser::StatelessSet::LimitExceptHandler
(const StatefulSet &data) const {
	parser_->IncrementArgNumber(data.GetRawData());
	return Parser::State::K_LIMIT_EXCEPT;
}

t_parsing_state Parser::StatelessSet::CgiAssignHandler
													(const StatefulSet &data) const {
	if (data.GetArgNumber() == 0) {
		parser_->IncrementArgNumber(data.GetRawData());
		return Parser::State::K_CGI_ASSIGN;
	} else if (data.GetArgNumber() == 1) {
		if (parser_->GetArgs()[0].size() < 2
			|| parser_->GetArgs()[0][0] != '.') {
			throw Analyser::SyntaxError("`cgi_assign' extension does not "
										"have the correct format", LINE);
		}
		const std::string extension = parser_->GetArgs()[0].substr(1);
		config_->AddCgiAssign(extension,
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
(const StatefulSet &data) const {
	if (data.GetArgNumber() == 0) {
		parser_->IncrementArgNumber(data.GetRawData());
		return Parser::State::K_RETURN;
	} else if (data.GetArgNumber() == 1) {
		char *endptr = NULL;
		int64_t status = std::strtol(parser_->GetArgs()[0].c_str(),
									 &endptr,
									 10);
		if ((endptr && *endptr) || errno || status < INT16_MIN || status > INT16_MAX
			|| !Constants::IsReturnStatusRedirection(static_cast<int16_t>(status)))
			throw Analyser::SyntaxError("Bad `return' status", LINE);
		if (data.GetRawData().find("http://") != 0
			&& data.GetRawData().find("https://") != 0) {
			throw Analyser::SyntaxError("Bad `return' URI", LINE);
		}
		if ((data.GetRawData().find("http://") == 0
			&& data.GetRawData().size() < (strlen("http://") + 1))
			|| (data.GetRawData().find("https://") == 0
				&& data.GetRawData().size() < (strlen("https://") + 1))) {
			throw Analyser::SyntaxError("Empty `return' URI", LINE);
		}
		config_->SetReturn(static_cast<uint16_t>(status),
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
													(const StatefulSet &data) const {
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
	const StatefulSet &data) const {
	char *endptr = NULL;
	int64_t result = std::strtol(data.GetRawData().c_str(), &endptr, 10);
	if ((endptr && *endptr) || result < 0 || UINT32_MAX < result || errno)
		throw SyntaxError("Invalid `client_max_body_size' directive "
						  "argument" , LINE);
	config_->SetClientMaxSz(static_cast<uint32_t>(result), data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::UploadStoreHandler(
	const StatefulSet &data) const {
	config_->SetUploadStore(data.GetRawData(), data.GetCtx(),
							LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::RootHandler(
	const StatefulSet &data) const {
	config_->SetRoot(data.GetRawData(), data.GetCtx(),
					 LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::IndexHandler(
	const StatefulSet &data) const {
	config_->SetIndex(data.GetRawData(), data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::LocationHandler(
	const StatefulSet &data) const {
	config_->AddLocation(data.GetRawData(), data.GetCtx(), LINE);
	parser_->PushContext(Parser::State::K_LOCATION);
	parser_->SkipEvent();
	return parser_->ParserMainLoop();
}

t_parsing_state Parser::StatelessSet::ListenHandler(
	const StatefulSet &data) const {
	std::string errorThrow;
	uint16_t port;
	uint32_t address;
	if (ParseIpAddressPort_(data.GetRawData(), &errorThrow, &port, &address))
		throw SyntaxError(errorThrow, LINE);
	config_->SetListenAddress(address, port, data.GetCtx(), LINE);
	return Parser::State::K_EXP_SEMIC;
}

t_parsing_state Parser::StatelessSet::ServerHandler(
	const StatefulSet &data) const {
	config_->AddServer(data.GetCtx(), LINE);
	parser_->PushContext(Parser::State::K_SERVER);
	return parser_->ParserMainLoop();
}
