#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <map>
#include <Connection.hpp>
#include <HttpRequest.hpp>
#include <HttpResponseFactory.hpp>
#include <ServerConfig.hpp>
#include <FDsets.hpp>

class Server {
	public:
		Server(const ServerConfig &settings, int listen_sd, FDsets *fdSets);
		~Server();
		void	AcceptNewConnection();
		bool	HasConnection(int sd);
		void	ReceiveRequest(int sd);
		void	SendResponse(int sd);

	private:
		Server();
		Server(const Server &);
		Server &	operator=(const Server &);
		void	AddConnection_(int sd);
		void	BindListeningSocket_();
		void	RemoveConnection_(int sd);

		ServerConfig			settings_;
		int						listen_sd_;
		FDsets					*fdSets_;
		std::map<int, Connection *>	connections_;
};

#endif  // SRCS_INCS_SERVER_HPP_
