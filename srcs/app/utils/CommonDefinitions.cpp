#include <CommonDefinitions.hpp>

bool Constants::IsValidMethod(const std::string &method) {
	const std::size_t	len =
		sizeof(kValidHttpMethods) / sizeof(kValidHttpMethods[0]);
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
