#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <map>
#include <Connection.hpp>
#include <ServerSettings.hpp>

class Server {
	private:
		ServerSettings			settings_;
		// Listening socket of this server
		int						listen_sd_;

		// The clients that are connected to the server
		// The key is the socket of the connection
		std::map<int, Connection>	connections_;

	public:
		explicit Server(const ServerSettings &settings);
		bool	BindListeningSocket();
		void	AddConnection(int sd);
		void	RemoveConnection(int sd);
		int		GetListeningSocket() const;
		bool	HaveConnection(int sd);
		bool	ReadRequest(int sd);
		bool	SendResponse(int sd);
};

#endif  // SRCS_INCS_SERVER_HPP_
