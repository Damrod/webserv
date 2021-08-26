#ifndef SRCS_INCS_WEBSERVER_HPP_
#define SRCS_INCS_WEBSERVER_HPP_
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <Config.hpp>
#include <Server.hpp>

class WebServer {
	private:
		Config	config_;
		std::vector<Server>	servers_;
		fd_set	master_set_;
		fd_set	read_set_;
		fd_set	write_set_;
		int		max_sd_;

	public:
		WebServer();
		// Load the config file and servers settings
		bool	Init(const std::string &pathname);

		// Run the web server
		void	Run();

	private:
		bool	PopulateServers_();
		void	AddListeningSocketsToMasterSet_();
		void	SetMaxSocket_(int curr_sd);
		std::vector<Server>::iterator	FindListeningSocket(int sd);
		void	AcceptNewConnection_(std::vector<Server>::iterator server_it);
		std::vector<Server>::iterator	FindConnectionServer(int sd);
		void	ReadRequest_(int sd);
		void	SendResponse_(int sd);
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
