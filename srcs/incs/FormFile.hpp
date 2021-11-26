#ifndef SRCS_INCS_FORMFILE_HPP_
#define SRCS_INCS_FORMFILE_HPP_
#include <string>
#include <stdexcept>
#include <Utils.hpp>
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

		void		ParseRequestContentType_(const HttpRequest &request);
		void		ParseRequestBody_(const HttpRequest &request);
		void		ParseFormHeaders_(const std::string &headers);
		void		ParseFormContentDisposition_(const std::string &header);
		std::string	ParseDoubleQuotedString_(
							const std::string &str, std::size_t *index) const;
		std::size_t	ParseHeaderName_(const std::string &str, std::size_t index,
									const std::string &name) const;
		std::size_t	ParseMediaType_(const std::string &str, std::size_t index,
									const std::string &name) const;
		void		ParseBoundary_(const std::string &str, std::size_t index);
		std::size_t	SkipWord_(const std::string &str, std::size_t index,
									const std::string &word) const;
		std::size_t	SkipWhitespace_(const std::string &str,
									std::size_t index) const;
		bool		IsValidFilename(const std::string &filename) const;
};

#endif  // SRCS_INCS_FORMFILE_HPP_
