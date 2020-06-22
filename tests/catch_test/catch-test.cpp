#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Input 0", "[none]")
{
	auto out = "";
	auto test = "";
	REQUIRE(out == test);
}

