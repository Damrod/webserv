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
#include <FDsets.hpp>
#include <parser/Analyser.hpp>
#include <Server.hpp>
#include <ServerConfig.hpp>
#include <SyscallWrap.hpp>

class WebServer {
	public:
		explicit	WebServer(const std::string &pathname);
		~WebServer();
		void	Run();

	private:
		WebServer();
		WebServer(const WebServer &);
		WebServer &	operator=(const WebServer &);
		void	PopulateServers_();
		Server	*FindServer_(int sd);
		Server	*FindServerWithConnection_(int sd);
		Server	*FindServerWithCgiHandler_(int sd);
		void	HandleReadSocket_(int sd);
		void	HandleWriteSocket_(int sd);

		typedef	int							Socket_;
		typedef std::map<Socket_, Server *>	ServersMap_;
		Config		config_;
		ServersMap_	servers_;
		FDsets		fdSets;

		typedef int							Fd_;
		typedef std::map<Fd_, CgiHandler *>	CgiHandlersMap_;
		typedef std::map<Socket_, Fd_>		CgiSocketFdsMap_;

		CgiHandlersMap_ 	cgi_handlers_;
		CgiSocketFdsMap_	cgi_fds_;
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
