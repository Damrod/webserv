#ifndef SRCS_INCS_CONNECTION_HPP_
#define SRCS_INCS_CONNECTION_HPP_
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <HttpRequest.hpp>
#include <HttpResponse.hpp>

class Connection {
	private:
		std::string	example_response_;
	private:
		int			socket_;
		//  HttpRequest	request_;
		//  HttpResponse response_;
		bool		ready_for_response_;

		std::string	raw_request_;
		std::string	raw_response_;

	public:
		explicit Connection(int sd);
		bool	ReadRequest();
		bool	SendResponse();
		// TODO(any):
		// Parse the raw_request
		// Generate the raw_response
};

#endif  // SRCS_INCS_CONNECTION_HPP_
