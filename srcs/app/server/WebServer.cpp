#include <WebServer.hpp>
#include <cstring>
#include <ServerConfig.hpp>

WebServer::WebServer() {
	FD_ZERO(&master_set_);
	FD_ZERO(&read_set_);
	FD_ZERO(&write_set_);
	max_sd_ = -1;
}

bool	WebServer::Init(const std::string &pathname) {
	// Load the config file
	if (!config_.LoadFile(pathname))
		return false;
	// Populate the servers
	if (!PopulateServers_())
		return false;
	return true;
}

void	WebServer::Run() {
	AddListeningSocketsToMasterSet_();
	while (max_sd_ != -1) {
		std::memcpy(&read_set_, &master_set_, sizeof(master_set_));
		std::memcpy(&write_set_, &master_set_, sizeof(master_set_));
		int ready_connections =
			select(max_sd_ + 1, &read_set_, &write_set_, NULL, NULL);
		for (int sd = 0; sd <= max_sd_ && ready_connections > 0; ++sd) {
			if (FD_ISSET(sd, &read_set_)) {
				--ready_connections;
				if (IsListeningSocket_(sd)) {
					AcceptNewConnection_(sd);
				} else {
					ReadRequest_(sd);
				}
			} else if (FD_ISSET(sd, &write_set_) && !IsListeningSocket_(sd)) {
				--ready_connections;
				SendResponse_(sd);
			}
		}
	}
}

bool	WebServer::PopulateServers_() {
	std::vector<ServerConfig>	servers_settings;

	servers_settings = config_.GetServersSettings();
	std::vector<ServerConfig>::iterator	settings_it = servers_settings.begin();

	while (settings_it != servers_settings.end()) {
		Server	server(*settings_it);

		if (!server.BindListeningSocket())
			return false;
		servers_.insert(std::make_pair(server.GetListeningSocket(), server));
		++settings_it;
	}
	return true;
}

void	WebServer::AddListeningSocketsToMasterSet_() {
	ServersMap_::iterator	server_it = servers_.begin();

	for (; server_it != servers_.end(); ++server_it) {
		int listen_sd = server_it->second.GetListeningSocket();
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

std::map<int, Server>::iterator
WebServer::FindListeningServer_(int sd) {
	return servers_.find(sd);
}

// Accept the new connection
// Add it to the server connections
// And to the master_set_
void	WebServer::AcceptNewConnection_(int sd) {
	ServersMap_::iterator	server_it = FindListeningServer_(sd);
	Server *server_ptr = &server_it->second;

	int new_sd = accept(server_ptr->GetListeningSocket(), NULL, NULL);
	fcntl(new_sd, F_SETFL, O_NONBLOCK);
	FD_SET(new_sd, &master_set_);
	server_ptr->AddConnection(new_sd);
	if (max_sd_ < new_sd)
		max_sd_ = new_sd;
}

std::map<int, Server>::iterator
WebServer::FindConnectionServer_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second.HasConnection(sd))
			return server_it;
	}
	return servers_.end();
}

// A connection is sending data
// Find which server has this connection
// And append the data to the connection HttpRequest
void	WebServer::ReadRequest_(int sd) {
	ServersMap_::iterator server_it = FindConnectionServer_(sd);
	Server *server_ptr = &server_it->second;

	if (!server_ptr->ReadRequest(sd)) {
		server_ptr->RemoveConnection(sd);
		FD_CLR(sd, &master_set_);
		SetMaxSocket_(sd);
	}
}

void	WebServer::SendResponse_(int sd) {
	ServersMap_::iterator server_it = FindConnectionServer_(sd);
	Server	*server_ptr = &server_it->second;

	if (!server_ptr->SendResponse(sd)) {
		server_ptr->RemoveConnection(sd);
		FD_CLR(sd, &master_set_);
		SetMaxSocket_(sd);
	}
}

bool	WebServer::IsListeningSocket_(int sd) const {
	return servers_.count(sd) > 0;
}
