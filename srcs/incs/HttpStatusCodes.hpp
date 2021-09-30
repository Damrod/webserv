#ifndef SRCS_INCS_HTTPSTATUSCODES_HPP_
#define SRCS_INCS_HTTPSTATUSCODES_HPP_
#include <map>
#include <string>

class HttpStatusCodes {
	private:
		typedef std::size_t								StatusCode_;
		typedef std::string								ReasonPhrase_;
		typedef std::map<StatusCode_, ReasonPhrase_>	StatusCodesMap_;
		static const StatusCodesMap_					kStatusCodesMap_;
		static const StatusCodesMap_					CreateStatusCodesMap_();

	public:
		static std::string	GetReasonPhrase(std::size_t status_code);
		static bool			IsValid(std::size_t status_code);
};

#endif  // SRCS_INCS_HTTPSTATUSCODES_HPP_
