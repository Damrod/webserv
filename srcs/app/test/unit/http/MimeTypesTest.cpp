#include <string>
#include <MimeTypes.hpp>
#include <catch2.hpp>

TEST_CASE("MimeTypes", "[http]") {
	REQUIRE("text/html" == MimeTypes::GetMimeType("html"));
	REQUIRE("text/plain" == MimeTypes::GetMimeType("txt"));
	REQUIRE("image/jpeg" == MimeTypes::GetMimeType("jpg"));
	REQUIRE("application/octet-stream" == MimeTypes::GetMimeType("unknown"));
}
