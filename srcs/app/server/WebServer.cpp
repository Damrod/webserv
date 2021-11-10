#include <WebServer.hpp>

WebServer::WebServer(const std::string &pathname) {
	max_sd_ = -1;
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
	AddListeningSocketsToMasterSet_();
	while (max_sd_ != -1) {
		int ready_sockets =
			select(max_sd_ + 1, fdSets.getReadSet(), fdSets.getWriteSet(), NULL, NULL);
		if (ready_sockets < 0) {
			throw std::runtime_error(std::strerror(errno));
		} else if (ready_sockets == 0) {
			throw std::runtime_error("select returned 0 with NULL timeout");
		}
		for (int sd = 0; sd <= max_sd_ && ready_sockets > 0; ++sd) {
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

int		WebServer::BindNewListeningSocketToServer_(const ServerConfig &settings) {
	int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	if (fcntl(listen_sd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  // IPv4
	addr.sin_port = htons(settings.listen_port);
	addr.sin_addr.s_addr = htonl(settings.listen_address);
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	int on = 1;
	if (setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	if (bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	if (listen(listen_sd, SOMAXCONN) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	return listen_sd;
}

void	WebServer::PopulateServers_() {
	std::vector<ServerConfig>	servers_settings;

	servers_settings = config_.GetServersSettings();
	std::vector<ServerConfig>::iterator	settings_it = servers_settings.begin();

	while (settings_it != servers_settings.end()) {
		int listen_sd = BindNewListeningSocketToServer_(*settings_it);

		Server	*server = new Server(*settings_it, listen_sd);
		servers_.insert(std::make_pair(listen_sd, server));
		++settings_it;
	}
}

void	WebServer::AddListeningSocketsToMasterSet_() {
	ServersMap_::iterator	server_it = servers_.begin();

	for (; server_it != servers_.end(); ++server_it) {
		int listen_sd = server_it->second->GetListeningSocket();
		fdSets.addToReadSet(listen_sd);
		if (max_sd_ < listen_sd) {
			max_sd_ = listen_sd;
		}
	}
}

void	WebServer::SetMaxSocket_(int curr_sd) {
	if (curr_sd == max_sd_) {
		while (!fdSets.isReadSet(max_sd_)) {
			--max_sd_;
		}
	}
}

Server	*WebServer::FindListeningServer_(int sd) {
	std::map<int, Server *>::iterator server_it;
	server_it = servers_.find(sd);
	if (server_it == servers_.end()) {
		return NULL;
	}
	return server_it->second;
}

void	WebServer::AcceptNewConnection_(int sd) {
	Server *server = FindListeningServer_(sd);

	int new_sd = accept(server->GetListeningSocket(), NULL, NULL);
	if (new_sd < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	if (fcntl(new_sd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	fdSets.addToReadSet(new_sd);
	server->AddConnection(new_sd);
	if (max_sd_ < new_sd) {
		max_sd_ = new_sd;
	}
}

Server	*WebServer::FindConnectionServer_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second->HasConnection(sd)) {
			return server_it->second;
		}
	}
	return NULL;
}

bool	WebServer::IsListeningSocket_(int sd) const {
	return servers_.count(sd) > 0;
}

void	WebServer::HandleReadSocket_(int sd) {
	if (IsListeningSocket_(sd)) {
		AcceptNewConnection_(sd);
	} else {
		Server	*server = FindConnectionServer_(sd);

		if (server) {
			ReceiveRequestStatus::Type status = server->ReceiveRequest(sd);
			if (status == ReceiveRequestStatus::kComplete) {
				fdSets.addToWriteSet(sd);
			} else if (status == ReceiveRequestStatus::kFail) {
				server->RemoveConnection(sd);
				fdSets.removeFromReadSet(sd);
				fdSets.removeFromWriteSet(sd);
				SetMaxSocket_(sd);
			}
		}
	}
}

void	WebServer::HandleWriteSocket_(int sd) {
	Server	*server = FindConnectionServer_(sd);

	if (server) {
		SendResponseStatus::Type status = server->SendResponse(sd);
		if (status == SendResponseStatus::kCompleteKeep) {
			fdSets.removeFromWriteSet(sd);
		} else if (status == SendResponseStatus::kFail ||
					status == SendResponseStatus::kCompleteClose) {
			server->RemoveConnection(sd);
			fdSets.removeFromReadSet(sd);
			fdSets.removeFromWriteSet(sd);
			SetMaxSocket_(sd);
		}
	}
}
