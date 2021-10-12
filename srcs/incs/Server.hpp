#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <map>
#include <Connection.hpp>
#include <ServerConfig.hpp>

class Server {
	private:
		ServerConfig			settings_;
		// Listening socket of this server
		int						listen_sd_;

		// The clients that are connected to the server
		// The key is the socket of the connection
		std::map<int, Connection>	connections_;

	public:
		explicit Server(const ServerConfig &settings);
		void	BindListeningSocket();
		void	AddConnection(int sd);
		void	RemoveConnection(int sd);
		int		GetListeningSocket() const;
		bool	HasConnection(int sd);

		enum ReadRequestStatus::Type	ReadRequest(int sd);
		enum SendResponseStatus::Type	SendResponse(int sd);
};

#endif  // SRCS_INCS_SERVER_HPP_
