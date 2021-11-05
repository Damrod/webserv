#include <WebServer.hpp>

WebServer::WebServer(const std::string &pathname) {
	FD_ZERO(&all_set_);
	FD_ZERO(&write_set_);
	max_sd_ = -1;
	config_.LoadFile(pathname);
	PopulateServers_();
}

WebServer::~WebServer() {
	ServersMap_::iterator it = servers_.begin();
	for (; it != servers_.end(); ++it) {
		delete it->second;
	}
	CgiHandlersMap_::iterator h_it = cgi_handlers_.begin();
	for (; h_it != cgi_handlers_.end(); ++h_it) {
		delete h_it->second;
	}
}

void	WebServer::Run() {
	AddListeningSocketsToMasterSet_();
	while (max_sd_ != -1) {
		std::memcpy(&tmp_read_set_, &all_set_, sizeof(all_set_));
		std::memcpy(&tmp_write_set_, &write_set_, sizeof(all_set_));
		int ready_fds =
			select(max_sd_ + 1, &tmp_read_set_, &tmp_write_set_, NULL, NULL);
		if (ready_fds < 0) {
			throw std::runtime_error(std::strerror(errno));
		}
		for (int sd = 0; sd <= max_sd_ && ready_fds > 0; ++sd) {
			if (FD_ISSET(sd, &tmp_read_set_)) {
				--ready_fds;
				HandleReadFd_(sd);
			} else if (FD_ISSET(sd, &tmp_write_set_)) {
				--ready_fds;
				HandleWriteFd_(sd);
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
		SafeSetSocket_(listen_sd, &all_set_);
	}
}

void	WebServer::SetMaxSocket_(int curr_sd) {
	if (curr_sd == max_sd_) {
		while (FD_ISSET(max_sd_, &all_set_) == 0) {
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
	server->AddConnection(new_sd);
	SafeSetSocket_(new_sd, &all_set_);
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

void	WebServer::HandleReadFd_(int sd) {
	if (IsListeningSocket_(sd)) {
		AcceptNewConnection_(sd);
	} else if (IsCgiFd_(sd)) {
		HandleCgiRead_(sd);
	} else {
		Server	*server = FindConnectionServer_(sd);

		if (server) {
			ReceiveRequestStatus::Type status = server->ReceiveRequest(sd);
			if (status == ReceiveRequestStatus::kComplete) {
				FD_SET(sd, &write_set_);
			} else if (status == ReceiveRequestStatus::kFail) {
				ClearSocket_(sd);
				server->RemoveConnection(sd);
			}
		}
	}
}

void	WebServer::HandleWriteFd_(int sd) {
	Server	*server = FindConnectionServer_(sd);

	if (server) {
		SendResponseStatus::Type status = server->SendResponse(sd);
		if (status == SendResponseStatus::kCompleteKeep) {
			FD_CLR(sd, &write_set_);
		} else if (status == SendResponseStatus::kHandleCgi) {
			AddCgiHandler_(server, sd);
		} else if (status == SendResponseStatus::kFail ||
					status == SendResponseStatus::kCompleteClose) {
			server->RemoveConnection(sd);
			ClearSocket_(sd);
		}
	} else if (IsCgiSocket_(sd)) {
		HandleCgiSend_(sd);
	}
}

void	WebServer::ClearSocket_(int sd) {
	if (FD_ISSET(sd, &all_set_)) {
		FD_CLR(sd, &all_set_);
	}
	if (FD_ISSET(sd, &write_set_)) {
		FD_CLR(sd, &write_set_);
	}
	SetMaxSocket_(sd);
}

void	WebServer::SafeSetSocket_(int sd, fd_set *fds) {
	if (!FD_ISSET(sd, fds)) {
		FD_SET(sd, fds);
		if (max_sd_ < sd) {
			max_sd_ = sd;
		}
	}
}

bool	WebServer::IsCgiFd_(int fd) const {
	return cgi_handlers_.count(fd) > 0;
}

bool	WebServer::IsCgiSocket_(int sd) const {
	return cgi_fds_.count(sd) > 0;
}

void	WebServer::RemoveCgiHandler_(CgiHandler *handler, int sd, int fd) {
	delete handler;
	cgi_fds_.erase(sd);
	cgi_handlers_.erase(fd);
}

void	WebServer::HandleCgiRead_(int fd) {
	CgiHandler *handler = cgi_handlers_[fd];
	int nbytes = handler->ReadCgiOutput();
	int handler_socket = handler->GetSocket();

	SafeSetSocket_(handler_socket, &write_set_);
	if (nbytes == 0) {
		// The cgi program has exit
		ClearSocket_(fd);
	} else if (nbytes < 0) {
		// There was an error while reading
		ClearSocket_(fd);
		ClearSocket_(handler_socket);
		RemoveCgiHandler_(handler, handler_socket, fd);
	}
}

void WebServer::HandleCgiSend_(int sd) {
	int cgi_output_fd = cgi_fds_[sd];
	CgiHandler *handler = cgi_handlers_[cgi_output_fd];
	int nbytes = handler->SendCgiOutput();

	if (nbytes <= 0) {
		ClearSocket_(sd);
		ClearSocket_(cgi_output_fd);
		RemoveCgiHandler_(handler, sd, cgi_output_fd);
	} else if (!handler->HasDataAvailable()) {
		ClearSocket_(sd);
		if (handler->IsCgiComplete()) {
			RemoveCgiHandler_(handler, sd, cgi_output_fd);
		}
	}
}

void WebServer::AddCgiHandler_(Server *server, int sd) {
	int cgi_output_fd = server->GetCgiOutputFd(sd);
	SafeSetSocket_(cgi_output_fd, &all_set_);

	int socket_copy = SyscallWrap::dupWr(sd);
	server->RemoveConnection(sd);
	ClearSocket_(sd);

	// Add a new handler for the cgi output;
	CgiHandler *handler = new CgiHandler(socket_copy, cgi_output_fd);
	cgi_handlers_.insert(std::make_pair(cgi_output_fd, handler));
	cgi_fds_.insert(std::make_pair(socket_copy, cgi_output_fd));
}
