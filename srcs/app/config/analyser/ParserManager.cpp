#include <parser/ParserManager.hpp>

ParserManager::ParserManager(const std::string &path) :
	servers_settings_(),
	path_(path),
	preprocessor_(path),
	filebuff_(preprocessor_.GetFileBuffer()),
	lexer_(filebuff_),
	tokens_(lexer_.GetTokens()),
	api_(&servers_settings_),
	parser_(tokens_, &api_) {}

std::vector<ServerConfig> ParserManager::GetServersSettings(void) {
	return servers_settings_;
}
