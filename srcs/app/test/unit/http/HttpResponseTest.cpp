#include <sstream>
#include <string>
#include <HttpResponse.hpp>
#include <catch2.hpp>

std::string	GetCurrentDate() {
	char				buffer[100];
	const std::time_t	date = std::time(NULL);
	std::strftime(buffer,
				sizeof(buffer),
				"%a, %d %b %Y %H:%M:%S %Z",
				std::gmtime(&date));
	return buffer;
}

TEST_CASE("InvalidHttpResponseInvalidStatus", "[http]") {
	HttpResponse::HeadersMap	headers;

	REQUIRE_THROWS(HttpResponse(42, headers, "", false, false));
}

TEST_CASE("ValidHttpResponseWithBody", "[http]") {
	const std::string	body = "Hello world!";
	std::stringstream	ss;
	ss << body.size();
	const std::string	body_size_str = ss.str();

	std::string	str = "HTTP/1.1 200 OK\r\n";

	HttpResponse::HeadersMap	headers;

	headers.insert(std::make_pair("Connection", "close"));
	headers.insert(std::make_pair("Date", GetCurrentDate()));
	headers.insert(std::make_pair("content-length", body_size_str));
	headers.insert(std::make_pair("content-type", "text/html"));
	headers.insert(std::make_pair("Server", "webserv"));
	HttpResponse::HeadersMap::const_iterator	it = headers.begin();
	HttpResponse::HeadersMap::const_iterator	ite = headers.end();
	while (it != ite) {
		str += it->first + ": " + it->second + "\r\n";
		++it;
	}
	str += "\r\n" + body;

	HttpResponse	response(200, headers, body, false, false);
	REQUIRE(str == response.RawContent());
}
