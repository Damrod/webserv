#include <WebServer.hpp>

WebServer::WebServer(const std::string &pathname) {
	config_.LoadFile(pathname);
	PopulateServers_();
}

WebServer::~WebServer() {
	ServersMap_::iterator it = servers_.begin();
	for (; it != servers_.end(); ++it) {
		delete it->second;
	}
}

void	WebServer::Run() {
	while (fdSets.getMaxSocket() != -1) {
		int ready_sockets =
			SyscallWrap::selectWr(
				fdSets.getMaxSocket() + 1,
				fdSets.getReadSet(),
				fdSets.getWriteSet(), NULL, NULL DEBUG_INFO);
		for (int sd = 0; sd <= fdSets.getMaxSocket() && ready_sockets > 0; ++sd) {
			if (fdSets.isReadSet(sd)) {
				--ready_sockets;
				HandleReadSocket_(sd);
			} else if (fdSets.isWriteSet(sd)) {
				--ready_sockets;
				HandleWriteSocket_(sd);
			}
		}
	}
}

void	WebServer::PopulateServers_() {
	std::vector<ServerConfig>	config;

	config = config_.GetServersSettings();
	std::vector<ServerConfig>::iterator	settings_it;

	while (!config.empty()) {
		settings_it = config.begin();
		int listen_sd = SyscallWrap::socketWr(AF_INET, SOCK_STREAM, 0 DEBUG_INFO);

		fdSets.addToReadSet(listen_sd);
		Server	*server =
					new Server(BuildServerSettings_(&config), listen_sd, &fdSets);
		servers_.insert(std::make_pair(listen_sd, server));
	}
}

WebServer::serverSettingsMap
	*WebServer::BuildServerSettings_(std::vector<ServerConfig> *config) {
	WebServer::serverSettingsMap *serverSettingsMap =
									new WebServer::serverSettingsMap();
	std::vector<ServerConfig>::iterator	it_config = config->begin();
	uint16_t	target_listen_port = it_config->listen_port;
	uint16_t	target_listen_address = it_config->listen_address;

	while (it_config != config->end()) {
		if (it_config->listen_address == target_listen_address
			&& it_config->listen_port == target_listen_port) {
			AddServerNamesToMap_(&(*it_config), serverSettingsMap);
			it_config = config->erase(it_config);
		} else {
			it_config++;
		}
	}
	return serverSettingsMap;
}

void	WebServer::AddServerNamesToMap_(
	ServerConfig *settings,
	WebServer::serverSettingsMap *serverSettingsMap) {
	ServerConfig *name_settings;
	std::vector<std::string> server_name = settings->server_name;
	std::vector<std::string>::iterator it_server_name = server_name.begin();
	bool default_server = serverSettingsMap->empty();

    if (server_name.empty()
		&& !serverSettingsMap->count("")) {
		name_settings = new ServerConfig(
								settings->listen_address,
								settings->listen_port,
								settings->common,
								default_server);
		serverSettingsMap->insert(std::make_pair("", name_settings));
	} else {
		DeleteDuplicatedServerNames_(&server_name, *serverSettingsMap);
 	   if (!server_name.empty()) {
		    for (; it_server_name != server_name.end(); it_server_name++) {
		    	std::string name = it_server_name->data();
				name_settings = new ServerConfig(
										settings->listen_address,
										settings->listen_port,
										settings->common,
										default_server);

			   	serverSettingsMap->insert(std::make_pair(name, name_settings));
		    }
 	   }
	}
}

void	WebServer::DeleteDuplicatedServerNames_(
					std::vector<std::string> *server_name,
					const WebServer::serverSettingsMap &serverSettingsMap) {
	if (!server_name->empty()) {
		std::vector<std::string>::iterator it = server_name->begin();

		while (it != server_name->end()) {
			if (serverSettingsMap.count(it->data())) {
				it = server_name->erase(it);
			} else {
				it++;
			}
		}
	}
}

Server	*WebServer::FindServer_(int sd) {
	std::map<int, Server *>::iterator server_it;
	server_it = servers_.find(sd);
	if (server_it == servers_.end()) {
		return NULL;
	}
	return server_it->second;
}

Server	*WebServer::FindServerWithConnection_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second->HasConnection(sd)) {
			return server_it->second;
		}
	}
	return NULL;
}

Server	*WebServer::FindServerWithCgiHandler_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second->HasCgiHandler(sd)) {
			return server_it->second;
		}
	}
	return NULL;
}

void	WebServer::HandleReadSocket_(int sd) {
	Server	*server;

	if ((server = FindServer_(sd))) {
		server->AcceptNewConnection();
	} else if ((server = FindServerWithConnection_(sd))) {
		server->ReceiveRequest(sd);
	} else if ((server = FindServerWithCgiHandler_(sd))) {
		server->HandleCgiRead(sd);
	}
}

void	WebServer::HandleWriteSocket_(int sd) {
	Server	*server;

	if ((server = FindServerWithConnection_(sd))) {
		server->SendResponse(sd);
	} else if ((server = FindServerWithCgiHandler_(sd))) {
		server->HandleCgiSend(sd);
	}
}
