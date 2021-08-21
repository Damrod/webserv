#define CATCH_CONFIG_MAIN
#include <catch.hpp>

int startup(int argc, char argv[3][3], char environ[3][3]);

TEST_CASE("Hello")
{
	char args[][3] = {"hi", "go", "to"};
	char env[][3] = {"in", "at", "an"};

	// args[0] = "23ad";

	GIVEN("some vars")
	{
		WHEN("we pass them to the function to test")
		{
			int a = startup(3, args, env);
			THEN("g ")
			{
				REQUIRE(a == 100);
			}
		}
	}
}
