#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <ServerConfig.hpp>

class Connection {
	public:
		Connection(const ServerConfig &server_config, int sd);
		bool	ReadRequest();
		bool	SendResponse();

	private:
		const ServerConfig	&server_config_;
		const int			socket_;
		bool				ready_for_response_;
		bool				keep_alive_;
		std::string			raw_request_;
		std::string			raw_response_;
};

#endif  // SRCS_INCS_CONNECTION_HPP_
