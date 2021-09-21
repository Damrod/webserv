#include <parser/ConfigSetters.hpp>

ParserAPI::ParserAPI(std::vector<ServerConfig> *server_settings) :
	servers_settings_(server_settings) {
}

std::vector<ServerConfig>	&ParserAPI::GetServersSettings(void) {
	return *servers_settings_;
}

void ParserAPI::SetListenPort(uint16_t port, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for port");
	servers_settings_->back().listen_port = port;
}

void ParserAPI::SetListenAddress(uint32_t address, t_parsing_state ctx_) {
	if (ctx_ != Token::State::K_SERVER)
		throw std::invalid_argument("Invalid context for listen address");
	servers_settings_->back().listen_address = address;
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
	CommonConfig common = GetLastCommonCfg(servers_settings_);
	Location location(path, common);
	servers_settings_->back().locations.push_back(location);
}

static std::stringstream &Indent(std::stringstream &o, uint8_t level) {
	for (size_t i = 0; i < level; ++i)
		o << "\t";
	return o;
}

static std::string printCommon(const CommonConfig &common, uint8_t lvl) {
	std::stringstream o;
	Indent(o, lvl);
	o << "root : " << common.root << "\n";
	Indent(o, lvl);
	o << "client_max_body_size : "
		  << common.client_max_body_size << "\n";
	Indent(o, lvl);
	o << "autoindex : " << common.autoindex << "\n";
	Indent(o, lvl);
	o << "index : " << common.index << "\n";
	Indent(o, lvl);
	o << "upload_store : " << common.upload_store << "\n";
	Indent(o, lvl);
	o << "return_status : " << common.return_status << "\n";
	Indent(o, lvl);
	o << "return_url : " << common.return_url << "\n";
	Indent(o, lvl);
	o << "error_pages map : " << "\n";
	for(CommonConfig::ErrorPagesMap::const_iterator
			iterr_pages = common.error_pages.begin();
		iterr_pages != common.error_pages.end();
		++iterr_pages) {
		Indent(o, lvl + 1);
		o << "error code: " << iterr_pages->first << ", error URI:"
		  << iterr_pages->second << "\n";
	}
	Indent(o, lvl);
	o << "cgi_assign map : " << "\n";
	for(CommonConfig::CgiAssignMap::const_iterator
			itcgi_ass = common.cgi_assign.begin();
		itcgi_ass != common.cgi_assign.end();
		++itcgi_ass) {
		Indent(o, lvl + 1);
		o << "file extension: " << itcgi_ass->first <<
			", binary handler path:"
		  << itcgi_ass->second << "\n";
	}
	return o.str();
}

std::ostream &operator<<(std::ostream &o, ParserAPI &c) {
	std::vector<ServerConfig>::iterator it =
		c.GetServersSettings().begin();
	for(size_t j = 0; it != c.GetServersSettings().end(); ++it, ++j) {
		o << "server " << j << ":\n";
		o << "\tlisten_address : " << it->listen_address << "\n";
		o << "\tlisten_port : " << it->listen_port << "\n";
		o << "\tserver_names :" << "\n";
		std::vector<std::string>::iterator itn = it->server_name.begin();
		for(size_t i = 0; itn != it->server_name.end(); ++itn, ++i) {
			o << "\t\tserver_name " << i << ": " << *itn << "\n";
		}
		o << printCommon(it->common, 1);
		o << "\tlocations :" << "\n";
		std::vector<Location>::iterator itl = it->locations.begin();
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

std::ostream &operator<<(std::ostream &o,
						 const std::vector<ServerConfig> &server_settings) {
	std::vector<ServerConfig>::const_iterator it =
		server_settings.begin();
	for(size_t j = 0; it != server_settings.end(); ++it, ++j) {
		o << "server " << j << ":\n";
		o << "\tlisten_address : " << it->listen_address << "\n";
		o << "\tlisten_port : " << it->listen_port << "\n";
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
