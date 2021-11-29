#include <CgiHandler.hpp>

CgiHandler::CgiHandler(int socket, int cgi_output)
	: socket_(socket), cgi_output_(cgi_output), cgi_complete_(false) {}

CgiHandler::~CgiHandler() {
	SyscallWrap::closeWr(socket_ DEBUG_INFO);
	SyscallWrap::closeWr(cgi_output_ DEBUG_INFO);
}

ssize_t	CgiHandler::ReadCgiOutput() {
	char	buffer[4096];

	ssize_t nbytes = read(cgi_output_, buffer, sizeof(buffer));
	if (nbytes > 0) {
		data_.append(&buffer[0], &buffer[nbytes]);
	}
	if (nbytes == 0) {
		cgi_complete_ = true;
	}
	return nbytes;
}

ssize_t	CgiHandler::SendCgiOutput() {
	ssize_t nbytes = send(socket_, data_.c_str(), data_.size(), 0);
	if (nbytes > 0) {
		data_.erase(0, nbytes);
	}
	return nbytes;
}

bool	CgiHandler::HasDataAvailable() const {
	return !data_.empty();
}

int		CgiHandler::GetSocket() const {
	return socket_;
}

int		CgiHandler::GetCgiOutFd() const {
	return cgi_output_;
}

bool	CgiHandler::IsCgiComplete() const {
	return cgi_complete_;
}
