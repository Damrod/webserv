#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <unistd.h>
#include <map>
#include <Connection.hpp>
#include <HttpRequest.hpp>
#include <HttpRequestHandler.hpp>
#include <ServerConfig.hpp>
#include <FDsets.hpp>

class Server {
	private:
		ServerConfig			settings_;
		// Listening socket of this server
		int						listen_sd_;
		FDsets					*fdSets_;

		// The clients that are connected to the server
		// The key is the socket of the connection
		std::map<int, Connection *>	connections_;

		Server();
		Server(const Server &);
		Server &	operator=(const Server &);

	public:
		Server(const ServerConfig &settings, int listen_sd, FDsets *fdSets);
		~Server();
		void	BindListeningSocket();
		void	AddConnection(int sd);
		void	RemoveConnection(int sd);
		int		GetListeningSocket() const;
		bool	HasConnection(int sd);

		void	ReceiveRequest(int sd);
		void	SendResponse(int sd);
};

#endif  // SRCS_INCS_SERVER_HPP_
