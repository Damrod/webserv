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
	std::vector<ServerConfig>::iterator	settings_it = config.begin();

	while (settings_it != config.end()) {
		int listen_sd = SyscallWrap::socketWr(AF_INET, SOCK_STREAM, 0 DEBUG_INFO);

		fdSets.addToReadSet(listen_sd);
		Server	*server = new Server(*settings_it, listen_sd, &fdSets);
		// Server	*server = new Server(buildServerSettings(config), listen_sd, &fdSets);
		servers_.insert(std::make_pair(listen_sd, server));
		++settings_it;
		// settings_it = config.begin();
	}
}

WebServer::serverSettingsMap
	WebServer::BuildServerSettings(std::vector<ServerConfig> &config) {
	WebServer::serverSettingsMap 		serverSettingsMap;
	std::vector<ServerConfig>::iterator	it_config = config.begin();
	uint16_t	target_listen_port = it_config->listen_port;
	uint16_t	target_listen_address = it_config->listen_address;

	while (it_config != config.end()) {
		if (it_config->listen_address == target_listen_address
			&& it_config->listen_port == target_listen_port) {
			AddServerNamesToMap_(*it_config, serverSettingsMap);
			it_config = config.erase(it_config);
		} else {
			it_config++;
		}
	}
	return serverSettingsMap;
}

void	WebServer::AddServerNamesToMap_(
	const ServerConfig &settings,
	WebServer::serverSettingsMap &serverSettingsMap) {
	// chequear cuando .empty()
	std::vector<std::string> server_name = settings.server_name;
	std::vector<std::string>::iterator it_server_name = server_name.begin();

	DeleteDuplicatedServerNames_(server_name, serverSettingsMap);
	for (; it_server_name != server_name.end(); it_server_name++) {
		std::string name = it_server_name->data();

		// ojo con referencias, puede no copiarse bien
		serverSettingsMap.insert(std::make_pair(name, settings));
	}
}

void	WebServer::DeleteDuplicatedServerNames_(
							std::vector<std::string> &server_name,
							WebServer::serverSettingsMap &serverSettingsMap) {
	if (!server_name.empty()) {
		std::vector<std::string>::iterator it = server_name.begin();

		while (it != server_name.end()) {
			if (serverSettingsMap.count(it->data())) {
				it = server_name.erase(it);
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
