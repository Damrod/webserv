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
				fdSets.getWriteSet(), NULL, NULL);
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
	std::vector<ServerConfig>	servers_settings;

	servers_settings = config_.GetServersSettings();
	std::vector<ServerConfig>::iterator	settings_it = servers_settings.begin();

	while (settings_it != servers_settings.end()) {
		int listen_sd = SyscallWrap::socketWr(AF_INET, SOCK_STREAM, 0);

		fdSets.addToReadSet(listen_sd);
		Server	*server = new Server(*settings_it, listen_sd, &fdSets);
		servers_.insert(std::make_pair(listen_sd, server));
		++settings_it;
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
