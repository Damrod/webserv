#include <parser/ConfigSetters.hpp>

ParserAPI::ParserAPI(std::vector<ServerConfig> *server_settings) :
	servers_settings_(server_settings) {
}

std::vector<ServerConfig>	&ParserAPI::GetServersSettings(void) {
	return *servers_settings_;
}

bool ParserAPI::canAddServer(uint32_t address, uint16_t port) {
	(void) address;
	(void) port;
	// mock
	return true;
}

bool ParserAPI::canAddLocation(const std::string &path) {
	(void) path;
	// mock
	return true;
}


void ParserAPI::SetListenAddress(uint32_t address, uint16_t port,
t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for listen address");
	if (canAddServer(address, port)) {
		servers_settings_->back().listen_address = address;
		servers_settings_->back().listen_port = port;
	} else {
		throw std::invalid_argument("duplicate default server for ");
	}
}

void ParserAPI::AddServerName(const std::string &name, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for server name");
	servers_settings_->back().server_name.push_back(name);
}

void ParserAPI::SetRoot(const std::string &root, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.root = root;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.root = root;
		else
			throw std::invalid_argument("Invalid context for root");
	}
}

void ParserAPI::AddIndex(const std::string &index, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.index = index;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.index = index;
		else
			throw std::invalid_argument("Invalid context for index");
	}
}

void ParserAPI::AddAutoindex(bool autoindex, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.autoindex = autoindex;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().locations.back().common.autoindex =
				autoindex;
		else
			throw std::invalid_argument("Invalid context for autoindex");
	}
}

void ParserAPI::SetClientMaxSz(uint32_t size, t_parsing_state ctx_) {
	if (ctx_ == Token::State::K_SERVER) {
		servers_settings_->back().common.client_max_body_size = size;
	} else {
		if (ctx_ == Token::State::K_LOCATION)
			servers_settings_->back().
				locations.back().common.client_max_body_size = size;
		else
			throw std::invalid_argument(
				"Invalid context for client_max_body_size");
	}
}

void ParserAPI::AddServer(t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_INIT)
		throw std::invalid_argument("Invalid context for server");
	ServerConfig server;
	servers_settings_->push_back(server);
}

static CommonConfig GetLastCommonCfg(std::vector<ServerConfig>
									 *servers_settings_) {
	CommonConfig config;
	config.root = servers_settings_->back().common.root;
	config.autoindex = servers_settings_->back().common.autoindex;
	config.client_max_body_size
		= servers_settings_->back().common.client_max_body_size;
	config.index = servers_settings_->back().common.index;
	config.error_pages = servers_settings_->back().common.error_pages;
	config.upload_store = servers_settings_->back().common.upload_store;
	config.return_status = servers_settings_->back().common.return_status;
	config.return_url = servers_settings_->back().common.return_url;
	config.cgi_assign = servers_settings_->back().common.cgi_assign;
	return config;
}

void ParserAPI::AddLocation(const std::string &path, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for location");
	if (canAddLocation(path)) {
		CommonConfig common = GetLastCommonCfg(servers_settings_);
		Location location(path, common);
		servers_settings_->back().locations.push_back(location);
	} else {
		throw std::invalid_argument("duplicate default server for ");
	}
}

static std::string printCommon(const CommonConfig &common, uint8_t lvl) {
	std::stringstream o;
	o << toStrIndented(lvl, "root", common.root);
	o << toStrIndented(lvl, "client_max_body_size",
					   common.client_max_body_size);
	o << toStrIndented(lvl, "autoindex", common.autoindex);
	o << toStrIndented(lvl, "index", common.index);
	o << toStrIndented(lvl, "upload_store", common.upload_store);
	o << toStrIndented(lvl, "return_status", common.return_status);
	o << toStrIndented(lvl, "return_url", common.return_url);
	o << MapToStrIndented(lvl, "error_pages map", common.error_pages,
	"error code", "error URI");
	o << MapToStrIndented(lvl, "cgi_assign map", common.cgi_assign,
	"file extension", "binary handler path");
	return o.str();
}

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings) {
	std::vector<ServerConfig>::const_iterator it =
		server_settings.begin();
	for(size_t j = 0; it != server_settings.end(); ++it, ++j) {
		o << "server " << j << ":\n";
		o << toStrIndented(1, "listen_address", it->listen_address);
		o << toStrIndented(1, "listen_port", it->listen_port);
		o << "\tserver_names :" << "\n";
		std::vector<std::string>::const_iterator itn = it->server_name.begin();
		for(size_t i = 0; itn != it->server_name.end(); ++itn, ++i) {
			o << "\t\tserver_name " << i << ": " << *itn << "\n";
		}
		o << printCommon(it->common, 1);
		o << "\tlocations :" << "\n";
		std::vector<Location>::const_iterator itl = it->locations.begin();
		for(size_t i = 0; itl != it->locations.end(); ++itl, ++i) {
			o << "\tpath " << i << ": "<< itl->path << "\n";
			o << "\t\tlimit except : \n";
			std::vector<Location::HttpMethod>::const_iterator itle =
				itl->limit_except.begin();
			for (size_t k = 0; itle != itl->limit_except.end();
				 ++itle, ++k)
				o << k << "\t\t\t" << *itle << "\n";
			o << printCommon(itl->common, 2);
		}
	}
	return o;
}
