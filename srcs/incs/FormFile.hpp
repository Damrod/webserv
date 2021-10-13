#ifndef SRCS_INCS_FORMFILE_HPP_
#define SRCS_INCS_FORMFILE_HPP_
#include <string>
#include <HttpRequest.hpp>

class FormFile {
	public:
		explicit FormFile(const HttpRequest &request);
		std::string	GetFile() const;

	private:
		std::string	filename_;
		std::string	file_content_;
		std::string	boundary_;

		void		ParseContentType_(const HttpRequest &request);
		void		ParseBody_(const HttpRequest &request);
};

#endif  // SRCS_INCS_FORMFILE_HPP_
