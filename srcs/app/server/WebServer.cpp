#include <WebServer.hpp>
#include <cerrno>
#include <cstring>
#include <queue>
#include <stdexcept>
#include <ServerConfig.hpp>

WebServer::WebServer() {
	FD_ZERO(&master_set_);
	FD_ZERO(&read_set_);
	FD_ZERO(&write_set_);
	max_sd_ = -1;
}

void	WebServer::Init(const std::string &pathname) {
	config_.LoadFile(pathname);
	PopulateServers_();
}

void	WebServer::Run() {
	AddListeningSocketsToMasterSet_();
	while (max_sd_ != -1) {
		std::memcpy(&read_set_, &master_set_, sizeof(master_set_));
		int ready_connections =
			select(max_sd_ + 1, &read_set_, &write_set_, NULL, NULL);
		if (ready_connections <= 0)
			throw std::runtime_error(std::strerror(errno));
		for (int sd = 0; sd <= max_sd_ && ready_connections > 0; ++sd) {
			if (FD_ISSET(sd, &read_set_)) {
				--ready_connections;
				std::vector<Server>::iterator
					server_it = FindListeningServer(sd);
				if (server_it != servers_.end()) {
					AcceptNewConnection_(server_it);
				} else {
					ReadRequest_(sd);
				}
			} else if (FD_ISSET(sd, &write_set_)) {
				--ready_connections;
				SendResponse_(sd);
			}
		}
	}
}

void	WebServer::PopulateServers_() {
	std::queue<ServerConfig>	servers_settings;

	servers_settings = config_.GetServersSettings();
	while (!servers_settings.empty()) {
		Server	server(servers_settings.front());
		servers_settings.pop();

		server.BindListeningSocket();
		servers_.push_back(server);
	}
}

void	WebServer::AddListeningSocketsToMasterSet_() {
	std::vector<Server>::const_iterator	server_it = servers_.begin();

	for (; server_it != servers_.end(); ++server_it) {
		int listen_sd = server_it->GetListeningSocket();
		FD_SET(listen_sd, &master_set_);
		if (max_sd_ < listen_sd)
			max_sd_ = listen_sd;
	}
}

void	WebServer::SetMaxSocket_(int curr_sd) {
    if (curr_sd == max_sd_) {
        while (FD_ISSET(max_sd_, &master_set_) == 0) {
            --max_sd_;
        }
    }
}

std::vector<Server>::iterator
WebServer::FindListeningServer(int sd) {
	std::vector<Server>::iterator	server_it = servers_.begin();

	for(; server_it != servers_.end(); ++server_it) {
		if (server_it->GetListeningSocket() == sd)
			return server_it;
	}
	return servers_.end();
}

// Accept the new connection and add it to the server connections
// And to the master_set_ and write_set_
void	WebServer::AcceptNewConnection_(
		std::vector<Server>::iterator server_it) {
	int new_sd = accept(server_it->GetListeningSocket(), NULL, NULL);
	if (new_sd < 0)
		throw std::runtime_error(std::strerror(errno));

	if (fcntl(new_sd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error(std::strerror(errno));
	FD_SET(new_sd, &master_set_);
	FD_SET(new_sd, &write_set_);
	server_it->AddConnection(new_sd);
	if (max_sd_ < new_sd)
		max_sd_ = new_sd;
}

std::vector<Server>::iterator
WebServer::FindConnectionServer(int sd) {
	std::vector<Server>::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->HaveConnection(sd))
			return server_it;
	}
	return servers_.end();
}

// A connection is sending data
// Find which server has this connection
// And append the data to the connection HttpRequest
void	WebServer::ReadRequest_(int sd) {
	std::vector<Server>::iterator server_it = FindConnectionServer(sd);

	if (!server_it->ReadRequest(sd)) {
		server_it->RemoveConnection(sd);
		FD_CLR(sd, &master_set_);
		SetMaxSocket_(sd);
	}
}

void	WebServer::SendResponse_(int sd) {
	std::vector<Server>::iterator server_it = FindConnectionServer(sd);
	if (!server_it->SendResponse(sd)) {
		server_it->RemoveConnection(sd);
		FD_CLR(sd, &master_set_);
		FD_CLR(sd, &write_set_);
		SetMaxSocket_(sd);
	}
}
