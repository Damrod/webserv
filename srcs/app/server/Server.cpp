#include <Server.hpp>

Server::Server(const ServerConfig &settings, int listen_sd)
	: settings_(settings), listen_sd_(listen_sd) {
}

Server::~Server() {
	std::map<int, Connection *>::iterator it = connections_.begin();
	for (; it != connections_.end(); it++) {
		close(it->first);
		delete it->second;
	}
	close(listen_sd_);
}

// Delegate how to handle connection responsability to Server
void	Server::AddConnection(int sd) {
	HttpRequestHandler *handler = new HttpRequestHandler(settings_);
	HttpRequest *request = new HttpRequest();
	Connection *connection = new Connection(sd, handler, request);
	connections_.insert(std::make_pair(sd, connection));
}

void	Server::RemoveConnection(int sd) {
	close(sd);
	delete connections_[sd];
	connections_.erase(sd);
}

int		Server::GetListeningSocket() const {
	return listen_sd_;
}

bool	Server::HasConnection(int sd) {
	return connections_.count(sd) > 0;
}

int	Server::GetCgiOutputFd(int sd) {
	return connections_[sd]->GetCgiOutputFd();
}

ReceiveRequestStatus::Type	Server::ReceiveRequest(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		return ReceiveRequestStatus::kFail;
	}
	return it->second->ReceiveRequest();
}

SendResponseStatus::Type	Server::SendResponse(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		return SendResponseStatus::kFail;
	}
	return it->second->SendResponse();
}
