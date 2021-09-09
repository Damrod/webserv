#include <map>
#include <string>
#include <HttpRequest.hpp>
#include <catch2.hpp>

TEST_CASE("ValidHttpGetRequestWithoutBody", "[http]") {
	const std::string raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
     	"Host: www.example.com\r\n"
	 	"Accept-Language: en, mi\r\n"
	 	"\r\n";

	HttpRequest	request(raw_request);
	REQUIRE("GET" == request.GetMethod());
	REQUIRE("/hello.txt" == request.GetRequestTarget());
	REQUIRE("HTTP/1.1" == request.GetHttpVersion());
	REQUIRE(!request.HasHeader("X-Custom-Header"));
	REQUIRE("www.example.com" == request.GetHeaderValue("Host"));
	REQUIRE("en, mi" == request.GetHeaderValue("Accept-Language"));
	REQUIRE("" == request.GetBody());
}

TEST_CASE("ValidHttpPostRequestWithBody", "[http]") {
	const std::string raw_request =
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

	HttpRequest	request(raw_request);
	REQUIRE("POST" == request.GetMethod());
	REQUIRE("/bin/login" == request.GetRequestTarget());
	REQUIRE("HTTP/1.1" == request.GetHttpVersion());
	REQUIRE(request.HasHeader("Host"));
	REQUIRE("image/gif, image/jpeg, */*" == request.GetHeaderValue("Accept"));
	REQUIRE("en-us" == request.GetHeaderValue("Accept-Language"));
	REQUIRE("gzip, deflate" == request.GetHeaderValue("Accept-Encoding"));
	REQUIRE("37" == request.GetHeaderValue("Content-Length"));
	REQUIRE("User=Peter+Lee&pw=123456&action=login" == request.GetBody());
}

TEST_CASE("InvalidHttpRequestNoHostThrowException", "[http]") {
	const std::string raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"\r\n";
	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("InvalidHttpRequestHeaderWithoutDelimiter", "[http]") {
	const std::string raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host 127.0.0.1:8000\r\n"
		"\r\n";
	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("InvalidHttpRequestRelativeRequestTarget", "[http]") {
	const std::string raw_request =
		"GET hello.txt HTTP/1.1\r\n"
		"Host: 127.0.0.1:8000\r\n"
		"\r\n";
	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("InvalidHttpRequestContentLengthWithoutBody", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 42\r\n"
		"\r\n";

	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("ValidHttpRequestZeroContentLengthWithoutBody", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 0\r\n"
		"\r\n";

	HttpRequest	request(raw_request);
	REQUIRE("0" == request.GetHeaderValue("Content-Length"));
}

TEST_CASE("InvalidHttpRequestContentLengthInvalidCharacters", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Length: 37abc\r\n"
		"\r\n"
		"User=Peter+Lee&pw=123456&action=login";

	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("ValidHttpRequestHostWithPort", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"\r\n";

	HttpRequest	request(raw_request);
	REQUIRE("localhost" == request.GetHost());
	REQUIRE(8080 == request.GetPort());
}

TEST_CASE("ValidHttpRequestHostWithoutPort", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n";

	HttpRequest	request(raw_request);
	REQUIRE("localhost" == request.GetHost());
	REQUIRE(80 == request.GetPort());
}

TEST_CASE("InvalidHttpRequestInvalidHostAndPort", "[http]") {
	const std::string	raw_request =
		"GET /hello.txt HTTP/1.1\r\n"
		"Host: :\r\n"
		"\r\n";

	REQUIRE_THROWS(HttpRequest(raw_request));
}

TEST_CASE("ValidHttpRequestWithQuery", "[http]") {
	const std::string	raw_request =
		"GET /test/demo_form.php?name1=value1&name2=value2;name3=value3 HTTP/1.1\r\n"
		"Host: www.example.com\r\n"
		"\r\n";

	HttpRequest	request(raw_request);
	REQUIRE("/test/demo_form.php" == request.GetPath());
	REQUIRE("value1" == request.GetQueryValue("name1"));
	REQUIRE("value2" == request.GetQueryValue("name2"));
	REQUIRE("value3" == request.GetQueryValue("name3"));
	REQUIRE(request.HasQuery("name1"));
	REQUIRE(!request.HasQuery("name42"));

	std::map<std::string, std::string>	query_map;
	query_map.insert(std::make_pair("name1", "value1"));
	query_map.insert(std::make_pair("name2", "value2"));
	query_map.insert(std::make_pair("name3", "value3"));
	REQUIRE(query_map == request.GetQueries());
}
