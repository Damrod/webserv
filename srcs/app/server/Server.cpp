#include <Server.hpp>

Server::Server(const ServerConfig &settings, int listen_sd, FDsets *fdSets)
	: settings_(settings), listen_sd_(listen_sd), fdSets_(fdSets) {
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
	fdSets_->removeFromReadSet(sd);
	fdSets_->removeFromWriteSet(sd);
	fdSets_->setMaxSocket(sd);
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

// Hay alguna manera de eliminar los estados?
// Si la conexion falla, como se notificaria a Server para que la elimine
// Server es responsable de sus conexiones
// Donde queda el CGI para pasarle los estados?

// Hacer removeConnection privado

void	Server::ReceiveRequest(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		RemoveConnection(sd);
	}
	ReceiveRequestStatus::Type status = it->second->ReceiveRequest();

	if (status == ReceiveRequestStatus::kComplete) {
		fdSets_->addToWriteSet(sd);
	} else if (status == ReceiveRequestStatus::kFail) {
		RemoveConnection(sd);
	}
}

void	Server::SendResponse(int sd) {
	std::map<int, Connection *>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		RemoveConnection(sd);
	}

	SendResponseStatus::Type status = it->second->SendResponse();
	if (status == SendResponseStatus::kCompleteKeep) {
		fdSets_->removeFromWriteSet(sd);
	} else if (status == SendResponseStatus::kFail ||
				status == SendResponseStatus::kCompleteClose) {
		RemoveConnection(sd);
	}
}
