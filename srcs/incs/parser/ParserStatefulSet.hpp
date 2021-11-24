#ifndef SRCS_INCS_PARSER_PARSERSTATEFULSET_HPP_
#define SRCS_INCS_PARSER_PARSERSTATEFULSET_HPP_

#include <string>
#include <parser/Analyser.hpp>
#include <parser/ParsingState.hpp>

namespace Parser {

class StatefulSet : public Analyser {
 public:
	StatefulSet(size_t line,
				State::e_id st,
				const std::string &rawData,
				State::e_id ctx,
				const char *error,
				size_t argNumber);
	State::e_id GetCtx(void) const;
	State::e_id GetState(void) const;
	const std::string &GetRawData(void) const;
	const char * GetErrorMessage(void) const;
	size_t GetLineNumber(void) const;
	size_t GetArgNumber(void) const;

 private:
	const char *error_msg_;
	const State::e_id state_;
	const std::string &rawData_;
	const State::e_id ctx_;
	const size_t argNumber_;
};

}  // namespace Parser

#endif  // SRCS_INCS_PARSER_PARSERSTATEFULSET_HPP_
