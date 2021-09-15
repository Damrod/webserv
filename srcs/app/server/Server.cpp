#include <Server.hpp>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <iostream>

Server::Server(const ServerConfig &settings)
	: settings_(settings), listen_sd_(-1) {
}

bool	Server::BindListeningSocket() {
	// TODO(any) Error checking
	listen_sd_ = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(listen_sd_, F_SETFL, O_NONBLOCK);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;  // IPv4
	addr.sin_port = htons(settings_.listen_port);
	addr.sin_addr.s_addr = htonl(settings_.listen_address);
	std::memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	int on = 1;
	setsockopt(listen_sd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	bind(listen_sd_, (struct sockaddr *)&addr, sizeof(addr));

	listen(listen_sd_, SOMAXCONN);  // 128
	return true;
}

void	Server::AddConnection(int sd) {
	std::cout << '[' << listen_sd_ << ']' <<
		" new connection on socket number: " << sd << '\n';
	connections_.insert(std::make_pair(sd, Connection(settings_, sd)));
}

void	Server::RemoveConnection(int sd) {
	std::cout << '[' << listen_sd_ << ']' <<
		" connection close on socket number: " << sd << '\n';
	close(sd);
	connections_.erase(sd);
}

int		Server::GetListeningSocket() const {
	return listen_sd_;
}

bool	Server::HasConnection(int sd) {
	return connections_.count(sd) > 0;
}

bool	Server::ReadRequest(int sd) {
	std::map<int, Connection>::iterator it = connections_.find(sd);
	if (it == connections_.end())
		return false;
	return it->second.ReadRequest();
}

bool	Server::SendResponse(int sd) {
	std::map<int, Connection>::iterator it = connections_.find(sd);
	if (it == connections_.end())
		return false;
	return it->second.SendResponse();
}
