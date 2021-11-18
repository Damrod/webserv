#include <Connection.hpp>

Connection::Connection(
					int socket,
					IResponseFactory *response_factory,
					IRequest *request) :
					socket_(socket),
					response_factory_(response_factory),
					request_(request),
					response_(NULL),
					keep_alive_(true),
					is_cgi_(false),
					cgi_output_fd_(-1) {}

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
		response_ = response_factory_->Response();
		raw_response_ = response_->Content();
		keep_alive_ = response_->KeepAlive();
		is_cgi_ = response_->IsCgi();
		if (is_cgi_) {
			cgi_output_fd_ = response_->GetCgiOutputFd();
		}
		delete response_;
		response_ = NULL;
	}
	ssize_t nbytes = send(socket_, raw_response_.c_str(), raw_response_.size(), 0);
	if (nbytes <= 0) {
		return SendResponseStatus::kFail;
	}
	raw_response_.erase(0, nbytes);
	if (raw_response_.empty()) {
		request_->Reset();
		if (is_cgi_) {
			return SendResponseStatus::kHandleCgi;
		}
		if (keep_alive_) {
			return SendResponseStatus::kCompleteKeep;
		}
		return SendResponseStatus::kCompleteClose;
	}
	return SendResponseStatus::kSuccess;
}

int	Connection::GetCgiOutputFd() const {
	return cgi_output_fd_;
}
