#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <ConnectionIOStatus.hpp>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <IRequest.hpp>
#include <ServerConfig.hpp>

class Connection {
	public:
		Connection(const ServerConfig &server_config, int sd);
		~Connection();
		ReadRequestStatus::Type		ReadRequest();
		SendResponseStatus::Type	SendResponse();

	private:
		Connection();
		Connection(const Connection &);
		Connection &	operator=(const Connection &);

		const ServerConfig	&server_config_;
		const int			socket_;
		bool				keep_alive_;
		IRequest			*request_;
		std::string			raw_request_;
		std::string			raw_response_;
};

#endif  // SRCS_INCS_CONNECTION_HPP_
