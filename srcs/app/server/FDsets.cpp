#include <FDsets.hpp>
#include <cstring>

FDsets::FDsets() {
	FD_ZERO(&read_set_);
	FD_ZERO(&write_set_);
}

void	FDsets::addToWriteSet(int fd) {
	FD_SET(fd, &write_set_);
}

void	FDsets::addToReadSet(int fd) {
	FD_SET(fd, &read_set_);
	if (max_fd_ < fd) {
		max_fd_ = fd;
	}
}

void	FDsets::removeFromWriteSet(int fd) {
	FD_CLR(fd, &write_set_);
}

void	FDsets::removeFd(int fd) {
	removeFromReadSet_(fd);
	removeFromWriteSet(fd);
	if (fd == max_fd_) {
		while (!isReadSet(max_fd_)) {
			--max_fd_;
		}
	}
}

int		FDsets::getMaxSocket() const {
	return max_fd_;
}

fd_set	*FDsets::getReadSet() {
	std::memcpy(&tmp_read_set_, &read_set_, sizeof(read_set_));
	return &tmp_read_set_;
}

fd_set	*FDsets::getWriteSet() {
	std::memcpy(&tmp_write_set_, &write_set_, sizeof(read_set_));
	return &tmp_write_set_;
}

bool	FDsets::isReadSet(int fd) const {
	return FD_ISSET(fd, &tmp_read_set_);
}

bool	FDsets::isWriteSet(int fd) const {
	return FD_ISSET(fd, &tmp_write_set_);
}


void	FDsets::removeFromReadSet_(int fd) {
	FD_CLR(fd, &read_set_);
}
