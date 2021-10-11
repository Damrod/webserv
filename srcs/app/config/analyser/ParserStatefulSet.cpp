#include <parser/Parser.hpp>

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
