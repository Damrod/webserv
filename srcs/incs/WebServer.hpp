#ifndef SRCS_INCS_WEBSERVER_HPP_
#define SRCS_INCS_WEBSERVER_HPP_
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <map>
#include <Config.hpp>
#include <ConnectionIOStatus.hpp>
#include <parser/Analyser.hpp>
#include <Server.hpp>
#include <ServerConfig.hpp>

class WebServer {
	private:
		typedef	int							Socket_;
		typedef std::map<Socket_, Server *>	ServersMap_;

		Config		config_;
		ServersMap_	servers_;
		fd_set		all_set_;
		fd_set		write_set_;
		fd_set		tmp_read_set_;
		fd_set		tmp_write_set_;
		int			max_sd_;

	public:
		// Load the config file and servers settings
		explicit	WebServer(const std::string &pathname);
		~WebServer();

		// Run the web server
		void	Run();

	private:
		WebServer();
		WebServer(const WebServer &);
		WebServer &	operator=(const WebServer &);

		void	PopulateServers_();
		void	AddListeningSocketsToMasterSet_();
		void	SetMaxSocket_(int curr_sd);
		void	AcceptNewConnection_(int sd);
		void	ReceiveRequest_(int sd);
		void	SendResponse_(int sd);
		bool	IsListeningSocket_(int sd) const;
		int		BindNewListeningSocketToServer_(const ServerConfig &settings);

		Server	*FindListeningServer_(int sd);
		Server	*FindConnectionServer_(int sd);

		void	HandleReadSocket_(int sd);
		void	HandleWriteSocket_(int sd);
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
