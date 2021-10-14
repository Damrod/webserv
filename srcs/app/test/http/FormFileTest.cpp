#include <FormFile.hpp>
#include <HttpRequest.hpp>
#include <catch2.hpp>

TEST_CASE("ValidFormFile", "[http]") {
	const std::string raw_request =
		"POST /foo HTTP/1.1\r\n"
		"Host: 127.0.0.1:8000\r\n"
		"Content-Length: 134\r\n"
		"Content-Type: multipart/form-data; boundary=something\r\n"
		"\r\n"
		"--something\r\n"
		"Content-Disposition: form-data; name=\"filename\"; filename=\"foo.txt\"\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n"
		"foobar\n\r\n"
		"--something--\r\n";

	HttpRequest request(raw_request);
	FormFile form_file(request);
	REQUIRE("foo.txt" == form_file.GetFilename());
	REQUIRE("foobar\n" == form_file.GetFileContent());
}
