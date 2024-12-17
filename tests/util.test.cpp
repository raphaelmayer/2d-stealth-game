#include "../src/util.hpp"
#include <catch2/catch.hpp>

TEST_CASE("to1d() should correctly convert 2D coordinates to 1D", "[util]")
{
	int width = 5;

	REQUIRE(to1d(2, 3, width) == 17);
	REQUIRE(to1d(2, 0, width) == 2);
	REQUIRE(to1d(0, 2, width) == 10);
}

TEST_CASE("to2d() should correctly convert 1D coordinates to 2D", "[util]")
{
	int width = 5;

	Vec2d result = to2d(13, width);
	REQUIRE(result.x == 3);
	REQUIRE(result.y == 2);

	Vec2d result2 = to2d(1, width);
	REQUIRE(result2.x == 1);
	REQUIRE(result2.y == 0);
}

TEST_CASE("toTileSize() should correctly convert integer values to tile size", "[util]")
{
	REQUIRE(toTileSize(0) == 0);
	REQUIRE(toTileSize(15) == 0);
	REQUIRE(toTileSize(16) == 1);
	REQUIRE(toTileSize(32) == 2);
}

TEST_CASE("toTileSize() should round correctly", "[util]")
{
	REQUIRE(toTileSize(1) == 0);
	REQUIRE(toTileSize(15) == 0);
	REQUIRE(toTileSize(22) == 1);
	REQUIRE(toTileSize(40) == 2);
}

TEST_CASE("toTileSize() should correctly convert Vec2d values to tile size", "[util]")
{
	Vec2d result = toTileSize({16, 32});
	REQUIRE(result.x == 1);
	REQUIRE(result.y == 2);
}

TEST_CASE("getTileIndex() should correctly determine the tile index for a given position", "[util]")
{
	Vec2d pos = {24, 35};

	REQUIRE(getTileIndex(pos) == 181);
}
