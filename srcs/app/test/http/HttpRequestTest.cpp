#include <string>
#include <HttpRequest.hpp>
#include <catch2.hpp>

TEST_CASE("ValidHttpGetRequestWithoutBody", "[http]") {
	std::string raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
     	"Host: www.example.com\r\n"
	 	"Accept-Language: en, mi\r\n"
	 	"\r\n";

	HttpRequest	request;
	REQUIRE(request.ParseRawString(raw_request));
	REQUIRE("GET" == request.GetMethod());
	REQUIRE("/hello.txt" == request.GetUri());
	REQUIRE("HTTP/1.1" == request.GetHttpVersion());
	REQUIRE(!request.HasHeader("X-Custom-Header"));
	REQUIRE("www.example.com" == request.GetHeaderValue("Host"));
	REQUIRE("en, mi" == request.GetHeaderValue("Accept-Language"));
	REQUIRE("" == request.GetBody());
}

TEST_CASE("ValidHttpPostRequestWithBody", "[http]") {
	std::string raw_request =
		"POST /bin/login HTTP/1.1\r\n"
		"Host: 127.0.0.1:8000\r\n"
		"Accept: image/gif, image/jpeg, */*\r\n"
		"Referer: http://127.0.0.1:8000/login.html\r\n"
		"Accept-Language: en-us\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n"
		"Content-Length: 37\r\n"
		"Connection: Keep-Alive\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n"
		"User=Peter+Lee&pw=123456&action=login";

	HttpRequest	request;
	REQUIRE(request.ParseRawString(raw_request));
	REQUIRE("POST" == request.GetMethod());
	REQUIRE("/bin/login" == request.GetUri());
	REQUIRE("HTTP/1.1" == request.GetHttpVersion());
	REQUIRE(request.HasHeader("Host"));
	REQUIRE("image/gif, image/jpeg, */*" == request.GetHeaderValue("Accept"));
	REQUIRE("en-us" == request.GetHeaderValue("Accept-Language"));
	REQUIRE("gzip, deflate" == request.GetHeaderValue("Accept-Encoding"));
	REQUIRE("37" == request.GetHeaderValue("Content-Length"));
	REQUIRE("User=Peter+Lee&pw=123456&action=login" == request.GetBody());
}
