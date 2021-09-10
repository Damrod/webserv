#include <Connection.hpp>
#include <iostream>

Connection::Connection(int socket) : socket_(socket) {
	// Temporary, until we implement HttpRequest and HttpResponse
	raw_response_ = "HTTP/1.1 200 OK\r\n"
								  "Server: hello_world\r\n"
								  "Content-Length: 22\r\n"
								  "Content-Type: text/html\r\n"
								  "\r\n"
								  "<h1>Hello, World!</h1>";
}

bool	Connection::ReadRequest() {
	// Temporary solution, until we implement the HttpRequest class
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0)
		return false;
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	return true;
}

bool	Connection::SendResponse() {
	// Temporary solution, until we implement the HttpResponse class
	int nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0)
		return false;
	raw_response_.erase(0, nbytes);
	return true;
}
