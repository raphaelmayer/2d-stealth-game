#include <catch2/catch.hpp>
#include "../../src/engine/Vec2d.hpp"

TEST_CASE("Vec2d Operations", "[Vec2d]")
{
	Vec2d vec1{3, 4};
	Vec2d vec2{1, 2};
	Vec2d vecZero{0, 0};

	SECTION("Equality and Inequality")
	{
		REQUIRE(vec1 == Vec2d{3, 4});
		REQUIRE_FALSE(vec1 == Vec2d{4, 3});
		REQUIRE(vec1 != Vec2d{4, 3});
	}

	SECTION("Addition")
	{
		REQUIRE((vec1 + vec2) == Vec2d{4, 6});
		REQUIRE((vec1 + 5) == Vec2d{8, 9});
		REQUIRE((vec1 + vecZero) == vec1);

		Vec2d vecAdd = vec1;
		vecAdd += vec2;
		REQUIRE(vecAdd == Vec2d{4, 6});
		vecAdd += 5;
		REQUIRE(vecAdd == Vec2d{9, 11});
	}

	SECTION("Subtraction")
	{
		REQUIRE((vec1 - vec2) == Vec2d{2, 2});
		REQUIRE((vec1 - 5) == Vec2d{-2, -1});
		REQUIRE((vec1 - vecZero) == vec1);

		Vec2d vecSub = vec1;
		vecSub -= vec2;
		REQUIRE(vecSub == Vec2d{2, 2});
		vecSub -= 5;
		REQUIRE(vecSub == Vec2d{-3, -3});
	}

	SECTION("Multiplication")
	{
		REQUIRE((vec1 * vec2) == Vec2d{3, 8});
		REQUIRE((vec1 * 2) == Vec2d{6, 8});
		REQUIRE((vec1 * vecZero) == vecZero);

		Vec2d vecMul = vec1;
		vecMul *= vec2;
		REQUIRE(vecMul == Vec2d{3, 8});
		vecMul *= 2;
		REQUIRE(vecMul == Vec2d{6, 16});
	}

	SECTION("Division")
	{
		REQUIRE((vec1 / Vec2d{1, 1}) == vec1);
		REQUIRE((vec1 / 2) == Vec2d{1, 2});

		Vec2d vecDiv = vec1;
		vecDiv /= Vec2d{1, 1};
		REQUIRE(vecDiv == vec1);
		vecDiv /= 2;
		REQUIRE(vecDiv == Vec2d{1, 2});
	}

	SECTION("to1d")
	{
		REQUIRE(vec1.to1d(10) == 43);
		// Additional tests for edge cases
	}

	SECTION("sign")
	{
		REQUIRE(vec1.sign() == Vec2d{1, 1});
		REQUIRE(Vec2d{-3, -4}.sign() == Vec2d{-1, -1});
		REQUIRE(vecZero.sign() == Vec2d{0, 0});
	}

	SECTION("toTileSize")
	{
		Vec2d vec = {32, 47};
		REQUIRE(vec.toTileSize().x == 2);
		REQUIRE(vec.toTileSize().y == 2);
		REQUIRE(vec1.toTileSize().x == 0);
		REQUIRE(vec1.toTileSize().y == 0);
	}
}