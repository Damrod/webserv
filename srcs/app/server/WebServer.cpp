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
	CgiHandlersMap_::iterator h_it = cgi_handlers_.begin();
	for (; h_it != cgi_handlers_.end(); ++h_it) {
		delete h_it->second;
	}
}

void	WebServer::Run() {
	while (fdSets.getMaxSocket() != -1) {
		int ready_sockets =
			select(
				fdSets.getMaxSocket() + 1,
				fdSets.getReadSet(),
				fdSets.getWriteSet(), NULL, NULL);
		if (ready_sockets < 0) {
			throw std::runtime_error(std::strerror(errno));
		}
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
		int listen_sd = socket(AF_INET, SOCK_STREAM, 0);
		if (listen_sd < 0) {
			throw std::runtime_error(std::strerror(errno));
		}

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

Server	*WebServer::FindServerConnection_(int sd) {
	ServersMap_::iterator	server_it = servers_.begin();

	for(; server_it!= servers_.end(); ++server_it) {
		if (server_it->second->HasConnection(sd)) {
			return server_it->second;
		}
	}
	return NULL;
}

void	WebServer::HandleReadSocket_(int sd) {
	Server	*server;

	if ((server = FindServer_(sd))) {
		server->AcceptNewConnection();
	} else if ((server = FindServerConnection_(sd))) {
		server->ReceiveRequest(sd);
	} else if (IsCgiFd_(sd)) {
		HandleCgiRead_(sd);
	}
}

void	WebServer::HandleWriteSocket_(int sd) {
	Server	*server = FindServerConnection_(sd);

	if (server) {
		SendResponseStatus::Type status = server->SendResponse(sd);
		if (status == SendResponseStatus::kHandleCgi) {
			AddCgiHandler_(server, sd);
		}
	} else if (IsCgiSocket_(sd)) {
		HandleCgiSend_(sd);
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

	fdSets.addToWriteSet(handler_socket);
	if (nbytes == 0) {
		// The cgi program has exit
		fdSets.removeFd(fd);
	} else if (nbytes < 0) {
		// There was an error while reading
		fdSets.removeFd(fd);
		fdSets.removeFd(handler_socket);
		RemoveCgiHandler_(handler, handler_socket, fd);
	}
}

void WebServer::HandleCgiSend_(int sd) {
	int cgi_output_fd = cgi_fds_[sd];
	CgiHandler *handler = cgi_handlers_[cgi_output_fd];
	int nbytes = handler->SendCgiOutput();

	if (nbytes <= 0) {
		fdSets.removeFd(sd);
		fdSets.removeFd(cgi_output_fd);
		RemoveCgiHandler_(handler, sd, cgi_output_fd);
	} else if (!handler->HasDataAvailable()) {
		fdSets.removeFd(sd);
		if (handler->IsCgiComplete()) {
			RemoveCgiHandler_(handler, sd, cgi_output_fd);
		}
	}
}

void WebServer::AddCgiHandler_(Server *server, int sd) {
	int cgi_output_fd = server->GetCgiOutputFd(sd);
	fdSets.addToReadSet(cgi_output_fd);

	int socket_copy = SyscallWrap::dupWr(sd);
	server->RemoveConnection(sd);

	// Add a new handler for the cgi output;
	CgiHandler *handler = new CgiHandler(socket_copy, cgi_output_fd);
	cgi_handlers_.insert(std::make_pair(cgi_output_fd, handler));
	cgi_fds_.insert(std::make_pair(socket_copy, cgi_output_fd));
}
