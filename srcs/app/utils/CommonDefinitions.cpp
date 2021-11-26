#include <CommonDefinitions.hpp>

const char Constants::kValidHttpMethods[8][8] = {"GET", "HEAD", "POST",
	"PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};

const uint16_t Constants::kRedirectionReturnStatus[5] = {301,
	302, 303, 307, 308};

const char Constants::kCRLF_[3] = "\r\n";
const char Constants::kWhitespace_[3] = " \t";

bool Constants::IsValidMethod(const std::string &method) {
	const std::size_t	len =
		sizeof(kValidHttpMethods) /
		sizeof(kValidHttpMethods[0]);
	for (std::size_t i = 0; i < len; ++i) {
		if (method == kValidHttpMethods[i])
			return true;
	}
	return false;
}

bool Constants::IsReturnStatusRedirection(int16_t status) {
	for (size_t i = 0;
		 i < sizeof(kRedirectionReturnStatus) /
			 sizeof(kRedirectionReturnStatus[0]);
		 ++i) {
		if (status == kRedirectionReturnStatus[i]) {
			return true;
		}
	}
	return false;
}
