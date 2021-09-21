#include <parser/ConfigSetters.hpp>

ConfigParserAPI::ConfigParserAPI(const std::string &path) : path_(path) {}

std::vector<ServerConfig> ConfigParserAPI::GetServersSettings(void) {
	Preprocessor file(path_);
	Lexer lexed(file.GetFileBuffer());
	std::list<Token> tokens = lexed.GetTokens();
	std::vector<ServerConfig> servers_settings;
	ParserAPI config(&servers_settings);
	Parser parser(tokens, &config);
	parser.parse();
	return config.GetServersSettings();
}
