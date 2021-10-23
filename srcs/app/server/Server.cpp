#include <Server.hpp>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

Server::Server(const ServerConfig &settings)
	: settings_(settings), listen_sd_(-1) {
}

void	Server::BindListeningSocket() {
	listen_sd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd_ < 0) {
		throw std::runtime_error(std::strerror(errno));
	}
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

void	Server::AddConnection(int sd) {
	connections_.insert(std::make_pair(sd, Connection(settings_, sd)));
}

void	Server::RemoveConnection(int sd) {
	close(sd);
	connections_.erase(sd);
}

int		Server::GetListeningSocket() const {
	return listen_sd_;
}

bool	Server::HasConnection(int sd) {
	return connections_.count(sd) > 0;
}

ReceiveRequestStatus::Type	Server::ReceiveRequest(int sd) {
	std::map<int, Connection>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		return ReceiveRequestStatus::kFail;
	}
	return it->second.ReceiveRequest();
}

SendResponseStatus::Type	Server::SendResponse(int sd) {
	std::map<int, Connection>::iterator it = connections_.find(sd);
	if (it == connections_.end()) {
		return SendResponseStatus::kFail;
	}
	return it->second.SendResponse();
}
