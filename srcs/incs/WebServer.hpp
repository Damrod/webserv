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
#include <FDsets.hpp>

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
		Server	*FindServerConnection_(int sd);
		void	HandleReadSocket_(int sd);
		void	HandleWriteSocket_(int sd);

		typedef	int							Socket_;
		typedef std::map<Socket_, Server *>	ServersMap_;
		Config		config_;
		ServersMap_	servers_;
		FDsets		fdSets;
};

#endif  // SRCS_INCS_WEBSERVER_HPP_
