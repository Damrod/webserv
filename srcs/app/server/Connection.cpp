#include <Connection.hpp>
#include <iostream>
#include <exception>
#include <HttpRequestHandler.hpp>

Connection::Connection(const ServerConfig &server_config, int socket)
	: server_config_(server_config), socket_(socket),
		ready_for_response_(false), raw_request_(""), raw_response_("") {
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
	if (raw_response_.empty()) {
		HttpRequestHandler	handler(server_config_, raw_request_);
		raw_request_.clear();
		raw_response_ = handler.GetRawResponse();
	}
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
