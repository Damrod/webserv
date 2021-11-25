#ifndef SRCS_INCS_FILE_HPP_
#define SRCS_INCS_FILE_HPP_

#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <MimeTypes.hpp>

class File {
 public:
    	explicit File(const std::string &file_path);
		std::string	GetPathExtension() const;
		std::string	GetPath() const;
		std::string	GetMimeType() const;
		std::string	GetContent();
		std::string	GetDirectoryContent();
		void	SetSubpath(std::string subpath);
		bool	IsRegularFile() const;
		bool	IsDirectory() const;
		bool	HasEndSlash() const;
		void	Upload(std::string upload_path, std::string content);

        class Error {
         public:
            	explicit	Error(int errCode): errCode_(errCode) {}
                int what() const throw() {
	             	return errCode_;
                }

         private:
                int errCode_;
        };

 private:
		void	SetPathExtension_();
		void	ThrowPathError_();

		std::string file_path_;
		std::string path_extension_;
		struct stat statbuf_;
};

#endif  // SRCS_INCS_FILE_HPP_
