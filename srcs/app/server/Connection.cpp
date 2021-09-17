#include <Connection.hpp>
#include <HttpRequestHandler.hpp>

Connection::Connection(const ServerConfig &server_config, int socket)
	: server_config_(server_config), socket_(socket),
		ready_for_response_(false), keep_alive_(true),
		raw_request_(""), raw_response_("") {
}

bool	Connection::ReadRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0)
		return false;
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
		keep_alive_ = handler.GetKeepAlive();
	}
	int nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0)
		return false;
	raw_response_.erase(0, nbytes);
	if (raw_response_.empty()) {
		ready_for_response_ = false;
		if (!keep_alive_)
			return false;
	}
	return true;
}
