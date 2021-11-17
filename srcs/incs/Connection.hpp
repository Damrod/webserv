#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <ConnectionIOStatus.hpp>
#include <HttpRequest.hpp>
#include <IRequest.hpp>
#include <IResponseFactory.hpp>
#include <ServerConfig.hpp>

class Connection {
	public:
		Connection(
				int sd,
				IResponseFactory *response_factory,
				IRequest *request);
		~Connection();
		ReceiveRequestStatus::Type	ReceiveRequest();
		SendResponseStatus::Type	SendResponse();
		int							GetCgiOutputFd() const;

	private:
		Connection();
		Connection(const Connection &);
		Connection &	operator=(const Connection &);

		const int			socket_;
		IResponseFactory	*response_factory_;
		IRequest			*request_;
		IResponse			*response_;
		bool				keep_alive_;
		bool				is_cgi_;
		int					cgi_output_fd_;
		std::string			raw_request_;
		std::string			raw_response_;
};

#endif  // SRCS_INCS_CONNECTION_HPP_
