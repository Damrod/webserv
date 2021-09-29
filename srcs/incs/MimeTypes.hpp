#ifndef SRCS_INCS_MIMETYPES_HPP_
#define SRCS_INCS_MIMETYPES_HPP_
#include <map>
#include <string>

class MimeTypes {
	public:
		static std::string			GetMimeType(const std::string &ext);

	private:
		typedef std::string							FileExtension_;
		typedef std::string							FileType_;
		typedef	std::map<FileExtension_, FileType_>	MimeTypesMap_;

		static const MimeTypesMap_	kMimeTypes_;
		static MimeTypesMap_		CreateMimeTypesMap_();
};

#endif  // SRCS_INCS_MIMETYPES_HPP_
