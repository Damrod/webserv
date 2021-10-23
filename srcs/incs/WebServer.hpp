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
#include <parser/Analyser.hpp>

class WebServer {
	private:
		typedef	int							Socket_;
		typedef std::map<Socket_, Server>	ServersMap_;

		Config		config_;
		ServersMap_	servers_;
		fd_set		all_set_;
		fd_set		write_set_;
		fd_set		tmp_read_set_;
		fd_set		tmp_write_set_;
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
		void	ReceiveRequest_(int sd);
		void	SendResponse_(int sd);
		bool	IsListeningSocket_(int sd) const;

		Server	*FindListeningServer_(int sd);
		Server  *FindConnectionServer_(int sd);
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
