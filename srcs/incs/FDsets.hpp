#ifndef SRCS_INCS_FDSETS_HPP_
#define SRCS_INCS_FDSETS_HPP_
#include <sys/select.h>
#include <sys/socket.h>
#include <cstring>

class FDsets {
	public:
		FDsets();
		void		addToWriteSet(int fd);
		void		addToReadSet(int fd);
		void		removeFd(int fd);
		void		removeFromWriteSet(int fd);
		int			getMaxSocket() const;
		fd_set		*getReadSet();
		fd_set		*getWriteSet();
		bool		isReadSet(int fd) const;
		bool		isWriteSet(int fd) const;
		bool		isAllReadSet(int fd) const;
		bool		isAllWriteSet(int fd) const;

	private:
		void		removeFromReadSet_(int fd);

		fd_set		read_set_;
		fd_set		tmp_read_set_;
		fd_set		write_set_;
		fd_set		tmp_write_set_;
		int			max_fd_;
};

#endif  // SRCS_INCS_FDSETS_HPP_
