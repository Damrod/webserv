#include <Connection.hpp>

Connection::Connection(
					int socket,
					IResponseFactory *response_factory,
					IRequest *request) :
					socket_(socket),
					response_factory_(response_factory),
					request_(request),
					response_(NULL),
					keep_alive_(true) {}

Connection::~Connection() {
	delete request_;
	if (response_) {
		delete response_;
	}
}

ReceiveRequestStatus::Type	Connection::ReceiveRequest() {
	char	buffer[4096];

	int nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0) {
		return ReceiveRequestStatus::kFail;
	}
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	if (raw_response_.empty() &&
							request_->GetState() == RequestState::kPartial) {
		request_->SetContent(raw_request_);
		raw_request_.erase(0, request_->ParsedOffset());
	}
	if (request_->GetState() == RequestState::kPartial) {
		return ReceiveRequestStatus::kSuccess;
	}
	return ReceiveRequestStatus::kComplete;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
		response_ = response_factory_->Response();
		raw_response_ = response_->Content();
		keep_alive_ = response_->KeepAlive();
		request_->Reset();
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
