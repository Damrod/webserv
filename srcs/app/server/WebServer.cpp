#include <WebServer.hpp>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <ConnectionIOStatus.hpp>
#include <ServerConfig.hpp>

WebServer::WebServer() {
	FD_ZERO(&all_set_);
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
		std::memcpy(&tmp_read_set_, &all_set_, sizeof(all_set_));
		std::memcpy(&tmp_write_set_, &write_set_, sizeof(all_set_));
		int ready_connections =
			select(max_sd_ + 1, &tmp_read_set_, &tmp_write_set_, NULL, NULL);
		if (ready_connections < 0) {
			throw std::runtime_error(std::strerror(errno));
		} else if (ready_connections == 0) {
			throw std::runtime_error("select returned 0 with NULL timeout");
		}
		for (int sd = 0; sd <= max_sd_ && ready_connections > 0; ++sd) {
			if (FD_ISSET(sd, &tmp_read_set_)) {
				--ready_connections;
				if (IsListeningSocket_(sd)) {
					AcceptNewConnection_(sd);
				} else {
					ReceiveRequest_(sd);
				}
			} else if (FD_ISSET(sd, &tmp_write_set_)) {
				--ready_connections;
				SendResponse_(sd);
                FD_CLR(sd, &write_set_);
			}
		}
	}
}

void	WebServer::PopulateServers_() {
	std::vector<ServerConfig>	servers_settings;

	servers_settings = config_.GetServersSettings();
	std::vector<ServerConfig>::iterator	settings_it = servers_settings.begin();

	while (settings_it != servers_settings.end()) {
		Server	server(*settings_it);

		server.BindListeningSocket();
		servers_.insert(std::make_pair(server.GetListeningSocket(), server));
		++settings_it;
	}
}

void	WebServer::AddListeningSocketsToMasterSet_() {
	ServersMap_::iterator	server_it = servers_.begin();

	for (; server_it != servers_.end(); ++server_it) {
		int listen_sd = server_it->second.GetListeningSocket();
		FD_SET(listen_sd, &all_set_);
		if (max_sd_ < listen_sd) {
			max_sd_ = listen_sd;
		}
	}
}

void	WebServer::SetMaxSocket_(int curr_sd) {
	if (curr_sd == max_sd_) {
		while (FD_ISSET(max_sd_, &all_set_) == 0) {
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
// And to the all_set_
void	WebServer::AcceptNewConnection_(int sd) {
	ServersMap_::iterator	server_it = FindListeningServer_(sd);
	Server *server_ptr = &server_it->second;

	int new_sd = accept(server_ptr->GetListeningSocket(), NULL, NULL);
	if (new_sd < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	if (fcntl(new_sd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	FD_SET(new_sd, &all_set_);
	server_ptr->AddConnection(new_sd);
	if (max_sd_ < new_sd) {
		max_sd_ = new_sd;
	}
}

std::map<int, Server>::iterator
WebServer::FindConnectionServer_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second.HasConnection(sd)) {
			return server_it;
		}
	}
	return servers_.end();
}

// A connection is sending data
// Find which server has this connection
// And append the data to the connection HttpRequest
void	WebServer::ReceiveRequest_(int sd) {
	ServersMap_::iterator server_it = FindConnectionServer_(sd);
	Server *server_ptr = &server_it->second;

	ReceiveRequestStatus::Type status = server_ptr->ReceiveRequest(sd);
	if (status == ReceiveRequestStatus::kStart) {
		FD_SET(sd, &write_set_);
	} else if (status == ReceiveRequestStatus::kFail) {
		server_ptr->RemoveConnection(sd);
		FD_CLR(sd, &all_set_);
		FD_CLR(sd, &write_set_);
		SetMaxSocket_(sd);
        return;
	}
    FD_SET(sd, &write_set_);
}

void	WebServer::SendResponse_(int sd) {
	ServersMap_::iterator server_it = FindConnectionServer_(sd);
	Server	*server_ptr = &server_it->second;

	SendResponseStatus::Type status = server_ptr->SendResponse(sd);
	if (status == SendResponseStatus::kCompleteKeep) {
		FD_CLR(sd, &write_set_);
	} else if (status == SendResponseStatus::kFail ||
				status == SendResponseStatus::kCompleteClose) {
		server_ptr->RemoveConnection(sd);
		FD_CLR(sd, &all_set_);
		FD_CLR(sd, &write_set_);
		SetMaxSocket_(sd);
	}
}

bool	WebServer::IsListeningSocket_(int sd) const {
	return servers_.count(sd) > 0;
}
