#include <Connection.hpp>

Connection::Connection(
					int socket,
					IResponseFactory *response_factory,
					IRequest *request) :
					socket_(socket),
					response_factory_(response_factory),
					request_(request),
					response_(NULL) {}

Connection::~Connection() {
	delete response_factory_;
	delete request_;
	delete response_;
}

ReceiveRequestStatus::Type	Connection::ReceiveRequest() {
	char	buffer[4096];

	ssize_t nbytes = recv(socket_, buffer, sizeof(buffer), 0);
	if (nbytes <= 0) {
		return ReceiveRequestStatus::kFail;
	}
	raw_request_.append(&buffer[0], &buffer[nbytes]);
	if (request_->IsPartial()) {
		request_->SetContent(raw_request_);
		raw_request_.erase(0, request_->ParsedOffset());
	}
	if (request_->IsPartial()) {
		return ReceiveRequestStatus::kSuccess;
	}
	return ReceiveRequestStatus::kComplete;
}

SendResponseStatus::Type	Connection::SendResponse() {
	if (raw_response_.empty()) {
		delete response_;
		response_ = response_factory_->Response();
		raw_response_ = response_->Content();
	}
	if (!response_->IsCgi()) {
		ssize_t nbytes =
				send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
		if (nbytes <= 0) {
			return SendResponseStatus::kFail;
		}
		raw_response_.erase(0, nbytes);
	}
	if (raw_response_.empty()) {
		request_->Reset();
		if (response_->IsCgi()) {
			return SendResponseStatus::kHandleCgi;
		}
		if (response_->KeepAlive()) {
			return SendResponseStatus::kCompleteKeep;
		}
		return SendResponseStatus::kCompleteClose;
	}
	return SendResponseStatus::kSuccess;
}

CgiInfo	Connection::GetCgiInfo() const {
	return response_->GetCgiInfo();
}
