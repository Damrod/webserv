#include <string>
#include <Utils.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TrimStringSameChars", "[utils]") {
	const std::string	str = "abcdcbaabcdcba";
	const std::string	trim_chars = "abcd";

	REQUIRE("" == TrimString(str, trim_chars));
}

TEST_CASE("TrimStringDifferentChars", "[utils]") {
	const std::string	str = " \t  Hello, World!\t\t ";
	const std::string	trim_chars = " \t";

	REQUIRE("Hello, World!" == TrimString(str, trim_chars));
}

TEST_CASE("ToLowerString", "[utils]") {
	const std::string	str = "Hello, World!";

	REQUIRE("hello, world!" == ToLowerString(str));
}

TEST_CASE("DecodeUrl", "[utils]") {
	const std::string	str = "%20http://hello+world!%20";

	REQUIRE(" http://hello world! " == DecodeUrl(str));
}
