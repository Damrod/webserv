#include <HttpStatusCodes.hpp>
#include <utility>

const char Constants::kValidHttpMethods[8][8] = {"GET", "HEAD", "POST",
	"PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"};
const uint16_t Constants::kValidReturnStatus[] = {301, 302, 303, 307, 308};

const HttpStatusCodes::StatusCodesMap_
HttpStatusCodes::kStatusCodesMap_ = CreateStatusCodesMap_();

const HttpStatusCodes::StatusCodesMap_
HttpStatusCodes::CreateStatusCodesMap_() {
	StatusCodesMap_ sc;

	sc.insert(std::make_pair(100, "Continue"));
	sc.insert(std::make_pair(101, "Switching Protocols"));
	sc.insert(std::make_pair(200, "OK"));
	sc.insert(std::make_pair(201, "Created"));
	sc.insert(std::make_pair(202, "Accepted"));
	sc.insert(std::make_pair(203, "Non-Authoritative Information"));
	sc.insert(std::make_pair(204, "No Content"));
	sc.insert(std::make_pair(205, "Reset Content"));
	sc.insert(std::make_pair(206, "Partial Content"));
	sc.insert(std::make_pair(300, "Multiple Choices"));
	sc.insert(std::make_pair(301, "Moved Permanently"));
	sc.insert(std::make_pair(302, "Found"));
	sc.insert(std::make_pair(303, "See Other"));
	sc.insert(std::make_pair(304, "Not Modified"));
	sc.insert(std::make_pair(305, "Use Proxy"));
	sc.insert(std::make_pair(307, "Temporary Redirect"));
	sc.insert(std::make_pair(400, "Bad Request"));
	sc.insert(std::make_pair(401, "Unauthorized"));
	sc.insert(std::make_pair(402, "Payment Required"));
	sc.insert(std::make_pair(403, "Forbidden"));
	sc.insert(std::make_pair(404, "Not Found"));
	sc.insert(std::make_pair(405, "Method Not Allowed"));
	sc.insert(std::make_pair(406, "Not Acceptable"));
	sc.insert(std::make_pair(407, "Proxy Authentication Required"));
	sc.insert(std::make_pair(408, "Request Timeout"));
	sc.insert(std::make_pair(409, "Conflict"));
	sc.insert(std::make_pair(410, "Gone"));
	sc.insert(std::make_pair(411, "Length Required"));
	sc.insert(std::make_pair(412, "Precondition Failed"));
	sc.insert(std::make_pair(413, "Payload Too Large"));
	sc.insert(std::make_pair(414, "URI Too Long"));
	sc.insert(std::make_pair(415, "Unsupported Media Type"));
	sc.insert(std::make_pair(416, "Range Not Satisfiable"));
	sc.insert(std::make_pair(417, "Expectation Failed"));
	sc.insert(std::make_pair(426, "Upgrade Required"));
	sc.insert(std::make_pair(500, "Internal Server Error"));
	sc.insert(std::make_pair(501, "Not Implemented"));
	sc.insert(std::make_pair(502, "Bad Gateway"));
	sc.insert(std::make_pair(503, "Service Unavailable"));
	sc.insert(std::make_pair(504, "Gateway Timeout"));
	sc.insert(std::make_pair(505, "HTTP Version Not Supported"));
	return sc;
}

std::string HttpStatusCodes::GetReasonPhrase(std::size_t status_code) {
	StatusCodesMap_::const_iterator it = kStatusCodesMap_.find(status_code);
	if (it != kStatusCodesMap_.end())
		return it->second;
	return "";
}

bool		HttpStatusCodes::IsValid(std::size_t status_code) {
	return kStatusCodesMap_.count(status_code) > 0;
}
