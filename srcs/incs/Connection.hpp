#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>

class Connection {
	private:
		int			socket_;
		//  HttpRequest	request_;
		//  HttpResponse response_;

		//  Temporary, until we implement HttpRequest and HttpResponse
		std::string	raw_request_;
		std::string raw_response_;

	public:
		explicit Connection(int sd);
		bool	ReadRequest();
		bool	SendResponse();
};

#endif  // SRCS_INCS_CONNECTION_HPP_
