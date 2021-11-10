#include <Server.hpp>

Server::Server(const ServerConfig &settings, int listen_sd, FDsets *fdSets)
	: settings_(settings), listen_sd_(listen_sd), fdSets_(fdSets) {
		BindListeningSocket_();
}

Server::~Server() {
	std::map<int, Connection *>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++) {
		close(it->first);
		delete it->second;
	}
	close(listen_sd_);
}

void	Server::BindListeningSocket_() {
	if (fcntl(listen_sd_, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  // IPv4
	addr.sin_port = htons(settings_.listen_port);
	addr.sin_addr.s_addr = htonl(settings_.listen_address);
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	int on = 1;
	if (setsockopt(listen_sd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	if (bind(listen_sd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}

	if (listen(listen_sd_, SOMAXCONN) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
}

void	Server::AddConnection_(int sd) {
	HttpRequestHandler *handler = new HttpRequestHandler(settings_);
	HttpRequest *request = new HttpRequest();
	Connection *connection = new Connection(sd, handler, request);
	connections_.insert(std::make_pair(sd, connection));
	fdSets_->addToReadSet(sd);
}

void	Server::RemoveConnection_(int sd) {
	fdSets_->removeFd(sd);
	close(sd);
	delete connections_[sd];
	connections_.erase(sd);
}

void	Server::AcceptNewConnection() {
	int new_sd = accept(listen_sd_, NULL, NULL);
	if (new_sd < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	if (fcntl(new_sd, F_SETFL, O_NONBLOCK) < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
	AddConnection_(new_sd);
}

bool	Server::HasConnection(int sd) {
	return connections_.count(sd) > 0;
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

void	Server::SendResponse(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	SendResponseStatus::Type status = it->second->SendResponse();
	if (status == SendResponseStatus::kCompleteKeep) {
		fdSets_->removeFromWriteSet(sd);
	} else if (status == SendResponseStatus::kFail ||
				status == SendResponseStatus::kCompleteClose) {
		RemoveConnection_(sd);
	}
}
