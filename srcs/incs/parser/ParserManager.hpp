#ifndef SRCS_INCS_PARSER_PARSERMANAGER_HPP_
#define SRCS_INCS_PARSER_PARSERMANAGER_HPP_

#include <arpa/inet.h>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <parser/Token.hpp>
#include <parser/Lexer.hpp>
#include <parser/Preprocessor.hpp>
#include <parser/ParserWrapper.hpp>
#include <parser/ParserEngine.hpp>
#include <ServerConfig.hpp>

class ParserManager {
 public:
	explicit ParserManager(const std::string &path);
	std::vector<ServerConfig> GetServersSettings(void) const;
 private:
	std::vector<ServerConfig> servers_settings_;
	const std::string path_;
	const Preprocessor preprocessor_;
	const std::string filebuff_;
	const Lexer lexer_;
	const std::list<Token> tokens_;
	Parser::Wrapper api_;
	const Parser::Engine parser_;
};

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings);

#endif  // SRCS_INCS_PARSER_PARSERMANAGER_HPP_
