#include <Connection.hpp>
#include <IRequestHandler.hpp>
#include <HttpRequestHandler.hpp>

Connection::Connection(const ServerConfig &server_config, int socket)
	: server_config_(server_config), socket_(socket), keep_alive_(true) {
}

ReadRequestStatus::Type	Connection::ReadRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0) {
		return ReadRequestStatus::kFail;
	}
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	if (raw_response_.empty() && request_.GetState() == HttpRequest::kPartial) {
		std::size_t offset = request_.ParseRawString(raw_request_);
		raw_request_.erase(0, offset);
	}
	HttpRequest::State request_state = request_.GetState();
	if (request_state == HttpRequest::kPartial)
		return ReadRequestStatus::kSuccess;
	return ReadRequestStatus::kComplete;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
		const bool is_complete = request_.GetState() == HttpRequest::kComplete;
		HttpRequest *request_ptr = is_complete ? &request_ : NULL;
		IRequestHandler *handler =
			new HttpRequestHandler(server_config_, request_ptr);
		raw_response_ = handler->GetRawResponse();
		keep_alive_ = handler->GetKeepAlive();
		delete handler;
		request_.Reset();
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
