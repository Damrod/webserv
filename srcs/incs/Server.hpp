#ifndef SRCS_INCS_SERVER_HPP_
#define SRCS_INCS_SERVER_HPP_
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <cstring>
#include <map>
#include <Connection.hpp>
#include <HttpRequest.hpp>
#include <HttpResponseFactory.hpp>
#include <ServerConfig.hpp>
#include <FDsets.hpp>
#include <SyscallWrap.hpp>
#include <CgiHandler.hpp>

class Server {
	public:
		Server(const ServerConfig &settings, int listen_sd, FDsets *fdSets);
		~Server();
		void	AcceptNewConnection();
		bool	HasConnection(int sd);
		bool	HasCgiHandler(int sd);
		void	ReceiveRequest(int sd);
		void	HandleCgiRead(int fd);
		void	SendResponse(int sd);
		void 	HandleCgiSend(int sd);

	private:
		Server();
		Server(const Server &);
		Server &	operator=(const Server &);
		void	AddConnection_(int sd);
		void	BindListeningSocket_() const;
		void	AddCgiHandler_(int sd, const CgiInfo &cgi_info);
		void	RemoveCgiHandler_(CgiHandler *handler, int sd, int fd);
		void	RemoveConnection_(int sd);

		typedef	int					Socket_;
		typedef	int					Fd_;
		typedef std::map<Fd_, CgiHandler *>	CgiHandlersMap_;
		typedef std::map<Socket_, Fd_>		CgiSocketFdsMap_;

		ServerConfig				settings_;
		int							listen_sd_;
		FDsets						*fdSets_;
		CgiHandlersMap_				cgi_handlers_;
		CgiSocketFdsMap_			cgi_fds_;
		std::map<int, Connection *>	connections_;
};

#endif  // SRCS_INCS_SERVER_HPP_
