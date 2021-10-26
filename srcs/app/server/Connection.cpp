#include <Connection.hpp>

Connection::Connection(int socket, IRequestHandler *requestHandler)
	: socket_(socket), requestHandler_(requestHandler), keep_alive_(true) {
}

Connection::~Connection() {
    delete requestHandler_;
}

ReceiveRequestStatus::Type	Connection::ReceiveRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0) {
		return ReceiveRequestStatus::kFail;
	}
	ReceiveRequestStatus::Type status = raw_request_.empty() ?
									ReceiveRequestStatus::kStart :
									ReceiveRequestStatus::kSuccess;
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	return status;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
        raw_response_ = requestHandler_->BuildResponse(raw_request_);
		raw_request_.clear();
		keep_alive_ = requestHandler_->GetKeepAlive();
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
