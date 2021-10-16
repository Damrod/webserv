#include <CommonDefinitions.hpp>

namespace Constants {

static const char kValidHttpMethods[8][8] = {"GET", "HEAD", "POST",
	"PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};

static const uint16_t kRedirectionReturnStatus[5] = {301,
	302, 303, 307, 308};

}  // namespace Constants

bool Constants::IsValidMethod(const std::string &method) {
	const std::size_t	len =
		sizeof(Constants::kValidHttpMethods) /
		sizeof(Constants::kValidHttpMethods[0]);
	for (std::size_t i = 0; i < len; ++i) {
		if (method == kValidHttpMethods[i])
			return true;
	}
	return false;
}

bool Constants::IsReturnStatusRedirection(int16_t status) {
	for (size_t i = 0;
		 i < sizeof(Constants::kRedirectionReturnStatus) /
			 sizeof(Constants::kRedirectionReturnStatus[0]);
		 ++i) {
		if (status == Constants::kRedirectionReturnStatus[i]) {
			return true;
		}
	}
	return false;
}
