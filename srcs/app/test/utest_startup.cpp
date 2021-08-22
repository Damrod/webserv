#define CATCH_CONFIG_MAIN
#include <catch2.hpp>

int startup(const std::vector<std::string> &argv,
			const std::vector<std::string> &env);

TEST_CASE("Hello") {
	std::vector<std::string> argv = {"hi", "go", "to"};
	std::vector<std::string> env = {"in", "at", "an"};

	GIVEN("some vars") {
		WHEN("we pass them to the function to test") {
			int a = startup(argv, env);
			THEN("The function returns whatever ") {
				REQUIRE(a == 10);
			}
		}
	}
}
