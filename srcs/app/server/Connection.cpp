#include <Connection.hpp>
#include <IRequestHandler.hpp>
#include <HttpRequestHandler.hpp>

Connection::Connection(const ServerConfig &server_config, int socket)
	: server_config_(server_config), socket_(socket), keep_alive_(true) {
}

enum ReadRequestStatus::Type	Connection::ReadRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0) {
		return ReadRequestStatus::kFail;
	}
	ReadRequestStatus::Type status = raw_request_.empty() ?
									ReadRequestStatus::kStart :
									ReadRequestStatus::kSuccess;
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	return status;
}

enum SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
		IRequestHandler *handler =
			new HttpRequestHandler(server_config_, raw_request_);
		raw_request_.clear();
		raw_response_ = handler->GetRawResponse();
		keep_alive_ = handler->GetKeepAlive();
		delete handler;
	}
	int nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0) {
		return SendResponseStatus::kFail;
	}
	raw_response_.erase(0, nbytes);
	if (raw_response_.empty()) {
		if (keep_alive_) {
			return SendResponseStatus::kCompleteKeep;
		} else {
			return SendResponseStatus::kCompleteClose;
		}
	}
	return SendResponseStatus::kSuccess;
}
