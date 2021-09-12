#include <Connection.hpp>
#include <iostream>

Connection::Connection(int socket) : socket_(socket) {
	// Temporary, until we implement HttpRequest and HttpResponse
	example_response_ = "HTTP/1.1 200 OK\r\n"
								  "Server: hello_world\r\n"
								  "Content-Length: 24\r\n"
								  "Connection: keep-alive\r\n"
								  "Content-Type: text/html\r\n"
								  "\r\n"
								  "<h1>Hello, World!!!</h1>";
}

bool	Connection::ReadRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0)
		return false;
	std::cout << "Received " << nbytes <<
	" bytes on socket number: " << socket_ << '\n';
	ready_for_response_ = true;
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	return true;
}

bool	Connection::SendResponse() {
	if (!ready_for_response_)
		return true;
	if (raw_response_.empty())
		// TODO(any):
		//    Create an HttpRequest using the raw_request
		//    Create the raw_response depending on the request method
		//    and the server config
		//
		// TODO(any):
		//    The Connection needs a pointer to the server config,
		//    we can pass it as a parameter of the constructor
		//
		// Temporary solution, until we implement the CreateHttpResponse() function
		raw_response_ = example_response_;

	int nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0)
		return false;
	std::cout << "Sent " << nbytes <<
	" bytes on socket number: " << socket_ << '\n';
	raw_response_.erase(0, nbytes);
	if (raw_response_.empty())
		ready_for_response_ = false;
	return true;
}
