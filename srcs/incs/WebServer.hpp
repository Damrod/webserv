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
#include <CgiHandler.hpp>
#include <Config.hpp>
#include <ConnectionIOStatus.hpp>
#include <parser/Analyser.hpp>
#include <Server.hpp>
#include <ServerConfig.hpp>
#include <SyscallWrap.hpp>

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

		typedef int							Fd_;
		typedef std::map<Fd_, CgiHandler *>	CgiHandlersMap_;
		typedef std::map<Socket_, Fd_>		CgiSocketFdsMap_;

		CgiHandlersMap_ 	cgi_handlers_;
		CgiSocketFdsMap_	cgi_fds_;

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
		void	ClearSocket_(int sd);
		void	SafeSetSocket_(int sd, fd_set *fds);

		Server	*FindListeningServer_(int sd);
		Server	*FindConnectionServer_(int sd);

		void	HandleReadFd_(int sd);
		void	HandleWriteFd_(int sd);

		bool	IsCgiFd_(int fd) const;
		bool	IsCgiSocket_(int sd) const;
		void	RemoveCgiHandler_(CgiHandler *handler, int sd, int fd);
		void	HandleCgiRead_(int fd);
		void	HandleCgiSend_(int sd);
		void	AddCgiHandler_(Server *server, int sd);
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
