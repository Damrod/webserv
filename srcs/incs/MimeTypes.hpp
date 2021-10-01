#ifndef SRCS_INCS_MIMETYPES_HPP_
#define SRCS_INCS_MIMETYPES_HPP_
#include <map>
#include <string>

class MimeTypes {
	public:
		static std::string			GetMimeType(const std::string &ext);

	private:
		// Map of file extensions and document types
		typedef	std::map<std::string, std::string>	MimeTypesMap_;

		static const MimeTypesMap_	kMimeTypes_;
		static const MimeTypesMap_	CreateMimeTypesMap_();

		// Disable copy construct and assign
		MimeTypes(const MimeTypes &);
		MimeTypes &	operator=(const MimeTypes &);
};

#endif  // SRCS_INCS_MIMETYPES_HPP_
