#ifndef SRCS_INCS_HTTPSTATUSCODES_HPP_
#define SRCS_INCS_HTTPSTATUSCODES_HPP_
#include <map>
#include <string>

class HttpStatusCodes {
	public:
		static std::string	GetReasonPhrase(std::size_t status_code);
		static bool			IsValid(std::size_t status_code);

	private:
		// Map of HTTP response status codes and reason phrases
		typedef std::map<std::size_t, std::string>		StatusCodesMap_;
		static const StatusCodesMap_					kStatusCodesMap_;
		static const StatusCodesMap_					CreateStatusCodesMap_();

		// Disable copy construct and assign
		HttpStatusCodes(const HttpStatusCodes &);
		HttpStatusCodes&	operator=(const HttpStatusCodes &);
};

#endif  // SRCS_INCS_HTTPSTATUSCODES_HPP_
