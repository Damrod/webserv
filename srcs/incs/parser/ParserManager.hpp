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
#include <parser/ParserAPI.hpp>
#include <parser/ParserEngine.hpp>
#include <ServerConfig.hpp>

class ParserManager {
 public:
	explicit ParserManager(const std::string &path);
	std::vector<ServerConfig> GetServersSettings(void);
 private:
	std::vector<ServerConfig> servers_settings_;
	std::string path_;
	Preprocessor preprocessor_;
	std::string filebuff_;
	Lexer lexer_;
	std::list<Token> tokens_;
	Parser::API api_;
	Parser::Engine parser_;
};

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings);

#endif  // SRCS_INCS_PARSER_PARSERMANAGER_HPP_
