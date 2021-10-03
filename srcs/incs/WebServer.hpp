#ifndef SRCS_INCS_WEBSERVER_HPP_
#define SRCS_INCS_WEBSERVER_HPP_
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <map>
#include <Config.hpp>
#include <Server.hpp>

class WebServer {
	private:
		typedef	int							Socket_;
		typedef std::map<Socket_, Server>	ServersMap_;

		Config		config_;
		ServersMap_	servers_;
		fd_set		master_set_;
		fd_set		read_set_;
		fd_set		write_set_;
		int			max_sd_;

	public:
		WebServer();
		// Load the config file and servers settings
		void	Init(const std::string &pathname);

		// Run the web server
		void	Run();

	private:
		void	PopulateServers_();
		void	AddListeningSocketsToMasterSet_();
		void	SetMaxSocket_(int curr_sd);
		void	AcceptNewConnection_(int sd);
		void	ReadRequest_(int sd);
		void	SendResponse_(int sd);
		bool	IsListeningSocket_(int sd) const;

		ServersMap_::iterator	FindListeningServer_(int sd);
		ServersMap_::iterator	FindConnectionServer_(int sd);
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
