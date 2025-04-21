#include <catch2/catch.hpp>
#include "../../src/engine/types/Vec2i.hpp"

TEST_CASE("Vec2i Operations", "[Vec2i]")
{
	Vec2i vec1{3, 4};
	Vec2i vec2{1, 2};
	Vec2i vecZero{0, 0};

	SECTION("Equality and Inequality")
	{
		REQUIRE(vec1 == Vec2i{3, 4});
		REQUIRE_FALSE(vec1 == Vec2i{4, 3});
		REQUIRE(vec1 != Vec2i{4, 3});
	}

	SECTION("Addition")
	{
		REQUIRE((vec1 + vec2) == Vec2i{4, 6});
		REQUIRE((vec1 + 5) == Vec2i{8, 9});
		REQUIRE((vec1 + vecZero) == vec1);

		Vec2i vecAdd = vec1;
		vecAdd += vec2;
		REQUIRE(vecAdd == Vec2i{4, 6});
		vecAdd += 5;
		REQUIRE(vecAdd == Vec2i{9, 11});
	}

	SECTION("Subtraction")
	{
		REQUIRE((vec1 - vec2) == Vec2i{2, 2});
		REQUIRE((vec1 - 5) == Vec2i{-2, -1});
		REQUIRE((vec1 - vecZero) == vec1);

		Vec2i vecSub = vec1;
		vecSub -= vec2;
		REQUIRE(vecSub == Vec2i{2, 2});
		vecSub -= 5;
		REQUIRE(vecSub == Vec2i{-3, -3});
	}

	SECTION("Multiplication")
	{
		REQUIRE((vec1 * vec2) == Vec2i{3, 8});
		REQUIRE((vec1 * 2) == Vec2i{6, 8});
		REQUIRE((vec1 * vecZero) == vecZero);

		Vec2i vecMul = vec1;
		vecMul *= vec2;
		REQUIRE(vecMul == Vec2i{3, 8});
		vecMul *= 2;
		REQUIRE(vecMul == Vec2i{6, 16});
	}

	SECTION("Division")
	{
		REQUIRE((vec1 / Vec2i{1, 1}) == vec1);
		REQUIRE((vec1 / 2) == Vec2i{1, 2});

		Vec2i vecDiv = vec1;
		vecDiv /= Vec2i{1, 1};
		REQUIRE(vecDiv == vec1);
		vecDiv /= 2;
		REQUIRE(vecDiv == Vec2i{1, 2});
	}

	SECTION("to1d")
	{
		REQUIRE(vec1.to1d(10) == 43);
		// Additional tests for edge cases
	}

	SECTION("sign")
	{
		REQUIRE(vec1.sign() == Vec2i{1, 1});
		REQUIRE(Vec2i{-3, -4}.sign() == Vec2i{-1, -1});
		REQUIRE(vecZero.sign() == Vec2i{0, 0});
	}
}