#include <parser/ConfigSetters.hpp>

static std::vector<ServerConfig> parse(const std::string &path) {
	Preprocessor file(path);
	Lexer lexed(file.GetFileBuffer());
	std::list<Token> tokens = lexed.GetTokens();
	std::vector<ServerConfig> servers_settings;
	ParserAPI config(&servers_settings);
	Parser::Engine parser(tokens, &config);
	return config.GetServersSettings();
}

ConfigParserAPI::ConfigParserAPI(const std::string &path) :
	servers_settings_(parse(path)),
	path_(path) {}

std::vector<ServerConfig> ConfigParserAPI::GetServersSettings(void) {
	return servers_settings_;
}
