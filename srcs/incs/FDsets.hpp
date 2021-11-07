#ifndef SRCS_INCS_FDSETS_HPP_
#define SRCS_INCS_FDSETS_HPP_

#include <sys/socket.h>

class FDsets {
	public:
		FDsets();
		void		removeFromReadSet(int fd);
		void		removeFromWriteSet(int fd);
		void		addToWriteSet(int fd);
		void		addToReadSet(int fd);
		int			getMaxSocket() const;
		fd_set		*getReadSet();
		fd_set		*getWriteSet();
		void		setMaxSocket(int curr_sd);
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;

	private:
		fd_set		read_set_;
		fd_set		tmp_read_set_;
		fd_set		write_set_;
		fd_set		tmp_write_set_;
		int			max_fd_;
};

#endif  // SRCS_INCS_FDSETS_HPP_
