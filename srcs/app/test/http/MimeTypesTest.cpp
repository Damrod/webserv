#include <string>
#include <MimeTypes.hpp>
#include <catch2.hpp>

TEST_CASE("MimeTypes", "[http]") {
	const MimeTypes	mime_types;

	REQUIRE("text/html" == mime_types.GetMimeType("html"));
	REQUIRE("text/plain" == mime_types.GetMimeType("txt"));
	REQUIRE("image/jpeg" == mime_types.GetMimeType("jpg"));
	REQUIRE("application/octet-stream" == mime_types.GetMimeType("unknown"));
}
