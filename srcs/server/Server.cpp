#include <Server.hpp>

Server::Server(Server::serverSettingsMap *settings,
				int listen_sd,
				FDsets *fdSets) :
				settings_(settings),
				listen_sd_(listen_sd),
				fdSets_(fdSets) {
		BindListeningSocket_();
}

Server::~Server() {
	std::map<int, Connection *>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++) {
		SyscallWrap::closeWr(it->first DEBUG_INFO);
		delete it->second;
	}
	CgiHandlersMap_::iterator h_it = cgi_handlers_.begin();
	for (; h_it != cgi_handlers_.end(); ++h_it) {
		delete h_it->second;
	}
	SyscallWrap::closeWr(listen_sd_ DEBUG_INFO);
	delete settings_;
}

void	Server::AcceptNewConnection() {
	int new_sd = SyscallWrap::acceptWr(listen_sd_, NULL, NULL DEBUG_INFO);
	SyscallWrap::fcntlWr(new_sd, F_SETFL, O_NONBLOCK DEBUG_INFO);
	AddConnection_(new_sd);
}

bool	Server::HasConnection(int sd) {
	return connections_.count(sd) > 0;
}

bool	Server::HasCgiHandler(int sd) {
	return cgi_handlers_.count(sd) > 0 ||
		   cgi_fds_.count(sd) > 0;
}

void	Server::ReceiveRequest(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	ReceiveRequestStatus::Type status = it->second->ReceiveRequest();
	if (status == ReceiveRequestStatus::kComplete) {
		fdSets_->addToWriteSet(sd);
	} else if (status == ReceiveRequestStatus::kFail) {
		RemoveConnection_(sd);
	}
}

void	Server::HandleCgiRead(int fd) {
	CgiHandler *handler = cgi_handlers_[fd];
	ssize_t nbytes = handler->ReadCgiOutput();
	int handler_socket = handler->GetSocket();

	if (nbytes < 0) {
		RemoveCgiHandler_(handler, handler_socket, fd);
	}
}

void	Server::SendResponse(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	SendResponseStatus::Type status = it->second->SendResponse();
	if (status == SendResponseStatus::kCompleteKeep) {
		fdSets_->removeFromWriteSet(sd);
	} else if (status == SendResponseStatus::kFail ||
				status == SendResponseStatus::kCompleteClose) {
		RemoveConnection_(sd);
	} else if (status == SendResponseStatus::kHandleCgi) {
		AddCgiHandler_(sd, it->second->GetCgiInfo());
	}
}

void Server::HandleCgiSend(int sd) {
	int cgi_output_fd = cgi_fds_[sd];
	CgiHandler *handler = cgi_handlers_[cgi_output_fd];
	ssize_t nbytes = handler->SendCgiOutput();

	if (nbytes <= 0 || handler->IsComplete()) {
		RemoveCgiHandler_(handler, sd, cgi_output_fd);
	}
}

void	Server::BindListeningSocket_() const {
	SyscallWrap::fcntlWr(listen_sd_, F_SETFL, O_NONBLOCK DEBUG_INFO);
	Server::serverSettingsMap::iterator it = settings_->begin();

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  // IPv4
	addr.sin_port = htons(it->second->listen_port);
	addr.sin_addr.s_addr = htonl(it->second->listen_address);
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	int on = 1;
	SyscallWrap::setsockoptWr(listen_sd_, SOL_SOCKET, SO_REUSEADDR, &on,
							  sizeof(on) DEBUG_INFO);
	SyscallWrap::bindWr(listen_sd_, (struct sockaddr *) &addr, sizeof(addr)
		DEBUG_INFO);
	SyscallWrap::listenWr(listen_sd_, SOMAXCONN DEBUG_INFO);
}

void	Server::AddConnection_(int sd) {
	HttpRequest *request = new HttpRequest(sd);
	HttpResponseFactory *response_factory =
							new HttpResponseFactory(request, settings_);
	Connection *connection = new Connection(sd, response_factory, request);
	connections_.insert(std::make_pair(sd, connection));
	fdSets_->addToReadSet(sd);
}

void	Server::RemoveConnection_(int sd) {
	fdSets_->removeFd(sd);
	SyscallWrap::closeWr(sd DEBUG_INFO);
	delete connections_[sd];
	connections_.erase(sd);
}

void	Server::AddCgiHandler_(int sd, const CgiInfo &cgi_info) {
	int socket_copy = SyscallWrap::dupWr(sd DEBUG_INFO);

	int cgi_output_fd = cgi_info.cgi_output_fd;
	CgiHandler *handler =
		new CgiHandler(fdSets_, socket_copy, cgi_info);
	cgi_handlers_.insert(std::make_pair(cgi_output_fd, handler));
	cgi_fds_.insert(std::make_pair(socket_copy, cgi_output_fd));
	RemoveConnection_(sd);
}

void	Server::RemoveCgiHandler_(CgiHandler *handler, int sd, int fd) {
	delete handler;
	cgi_fds_.erase(sd);
	cgi_handlers_.erase(fd);
}
