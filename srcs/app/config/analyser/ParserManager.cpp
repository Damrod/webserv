#include <parser/ParserManager.hpp>

static std::vector<ServerConfig> parse(const std::string &path) {
	Preprocessor file(path);
	Lexer lexed(file.GetFileBuffer());
	std::list<Token> tokens = lexed.GetTokens();
	std::vector<ServerConfig> servers_settings;
	Parser::ParserAPI config(&servers_settings);
	Parser::Engine parser(tokens, &config);
	return config.GetServersSettings();
}

ParserManager::ParserManager(const std::string &path) :
	servers_settings_(parse(path)),
	path_(path) {}

std::vector<ServerConfig> ParserManager::GetServersSettings(void) {
	return servers_settings_;
}
