#ifndef SRCS_INCS_FORMFILE_HPP_
#define SRCS_INCS_FORMFILE_HPP_
#include <string>
#include <HttpRequest.hpp>

class FormFile {
	public:
		explicit FormFile(const HttpRequest &request);
		std::string	GetFilename() const;
		std::string	GetFileContent() const;

	private:
		std::string	filename_;
		std::string	file_content_;
		std::string	boundary_;

		static const char kCRLF[];
		static const char kWhitespace[];

		void		ParseRequestContentType_(const HttpRequest &request);
		void		ParseRequestBody_(const HttpRequest &request);
		void		ParseFormHeaders_(const std::string &headers);
		void		ParseFormContentDisposition_(const std::string &header);
		void		ParseFormContentType_(const std::string &header) const;
		std::size_t	SkipSpace(const std::string &str, std::size_t index) const;
		std::string	ParseDoubleQuotedString(
							const std::string &str, std::size_t *index) const;
};

#endif  // SRCS_INCS_FORMFILE_HPP_
