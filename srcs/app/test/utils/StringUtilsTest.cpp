#include <string>
#include <StringUtils.hpp>
#include <catch2.hpp>

TEST_CASE("TrimStringSameChars", "[utils]") {
	std::string	str = "abcdcbaabcdcba";
	std::string	trim_chars = "abcd";

	REQUIRE("" == TrimString(str, trim_chars));
}

TEST_CASE("TrimStringDifferentChars", "[utils]") {
	std::string	str = " \t  Hello, World!\t\t ";
	std::string	trim_chars = " \t";

	REQUIRE("Hello, World!" == TrimString(str, trim_chars));
}

TEST_CASE("ToLowerString", "[utils]") {
	std::string	str = "Hello, World!";

	REQUIRE("hello, world!" == ToLowerString(str));
}
