#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <map>
#include <unistd.h>
#include <Connection.hpp>
#include <ServerConfig.hpp>
#include <HttpRequestHandler.hpp>

class Server {
	private:
		ServerConfig			settings_;
		// Listening socket of this server
		int						listen_sd_;

		// The clients that are connected to the server
		// The key is the socket of the connection
		std::map<int, Connection>	connections_;

	public:
		explicit Server(const ServerConfig &settings, int listen_sd);
		void	BindListeningSocket();
		void	AddConnection(int sd);
		void	RemoveConnection(int sd);
		int		GetListeningSocket() const;
		bool	HasConnection(int sd);

		ReceiveRequestStatus::Type	ReceiveRequest(int sd);
		SendResponseStatus::Type	SendResponse(int sd);
};

#endif  // SRCS_INCS_SERVER_HPP_
