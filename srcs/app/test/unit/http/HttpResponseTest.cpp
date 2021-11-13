#include <sstream>
#include <string>
#include <catch2.hpp>

TEST_CASE("InvalidHttpResponseInvalidStatus", "[http]") {
	REQUIRE_THROWS(HttpResponse(42));
}

TEST_CASE("ValidHttpResponseWithoutHeadersAndBody", "[http]") {
	const std::string str = "HTTP/1.1 200 OK\r\n"
							"\r\n";
	HttpResponse	response(200);

	REQUIRE("HTTP/1.1" == response.GetHttpVersion());
	REQUIRE(200 == response.GetStatusCode());
	REQUIRE("OK" == response.GetReasonPhrase());
	REQUIRE(str == response.CreateResponseString());
}

TEST_CASE("ValidHttpResponseWithoutBody", "[http]") {
	const std::string str = "HTTP/1.1 200 OK\r\n"
							"server: webserv\r\n"
							"\r\n";
	HttpResponse	response(200);

	response.AddHeader("Server", "webserv");
	REQUIRE("HTTP/1.1" == response.GetHttpVersion());
	REQUIRE(200 == response.GetStatusCode());
	REQUIRE("OK" == response.GetReasonPhrase());
	REQUIRE("webserv" == response.GetHeaderValue("Server"));
	REQUIRE(str == response.CreateResponseString());
}

TEST_CASE("ValidHttpResponseWithBody", "[http]") {
	const std::string	body = "Hello world!";
	std::stringstream	ss;
	ss << body.size();
	const std::string	body_size_str = ss.str();

	std::string	str = "HTTP/1.1 200 OK\r\n";

	typedef std::map<std::string, std::string>	HeadersMap;
	HeadersMap	headers;

	headers.insert(std::make_pair("server", "webserv"));
	headers.insert(std::make_pair("content-length", body_size_str));
	headers.insert(std::make_pair("content-type", "text/html"));
	HeadersMap::const_iterator	it = headers.begin();
	HeadersMap::const_iterator	ite = headers.end();
	while (it != ite) {
		str += it->first + ": " + it->second + "\r\n";
		++it;
	}
	str += "\r\n" + body;

	HttpResponse	response(200);

	response.AddHeader("Server", "webserv");
	response.AddHeader("Content-Length", body_size_str);
	response.AddHeader("Content-Type", "text/html");
	response.SetBody(body);
	REQUIRE("HTTP/1.1" == response.GetHttpVersion());
	REQUIRE(200 == response.GetStatusCode());
	REQUIRE("OK" == response.GetReasonPhrase());
	REQUIRE("webserv" == response.GetHeaderValue("Server"));
	REQUIRE(str == response.CreateResponseString());
	REQUIRE(headers == response.GetHeaders());
}
