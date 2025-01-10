#include "../../src/modules/AStar.hpp"
#include <catch2/catch.hpp>

// Example maps for testing
std::vector<std::vector<bool>> emptyMap = {
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
};

std::vector<std::vector<bool>> obstacleMap = {
    {0, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, {0, 1, 0, 1, 0}, {0, 0, 0, 0, 0},
};

std::vector<std::vector<bool>> blockedMap = {
    {0, 0, 1, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 1, 1, 0}, {0, 1, 1, 1, 0}, {0, 0, 1, 0, 0},
};

std::vector<std::vector<bool>> tightCorridors = {
    {0, 1, 1, 1, 1}, {0, 0, 1, 1, 1}, {1, 0, 0, 1, 1}, {1, 1, 0, 0, 1}, {1, 1, 1, 0, 0},
};

// Helper function to check path validity
void verifyPath(const std::vector<Vec2i> &path, const Vec2i &start, const Vec2i &end,
                const std::vector<std::vector<bool>> &map)
{
	REQUIRE_FALSE(path.empty());
	REQUIRE(path.front() == start);
	REQUIRE(path.back() == end);

	for (size_t i = 1; i < path.size(); ++i) {
		Vec2i delta = path[i] - path[i - 1];
		REQUIRE((std::abs(delta.x) <= 1 && std::abs(delta.y) <= 1)); // Path must be contiguous
	}

	for (const auto &position : path) {
		REQUIRE(map[position.y][position.x] == 0); // Path must avoid obstacles
	}
}

TEST_CASE("AStar Pathfinding Tests", "[AStar]")
{
	AStar aStar;

	SECTION("Simple Pathfinding in an Empty Map")
	{
		Vec2i start = {0, 0};
		Vec2i end = {4, 4};
		auto path = aStar.findPath(emptyMap, start, end);
		verifyPath(path, start, end, emptyMap);
	}

	SECTION("Pathfinding Around Obstacles")
	{
		Vec2i start = {0, 0};
		Vec2i end = {4, 4};
		auto path = aStar.findPath(obstacleMap, start, end);
		verifyPath(path, start, end, obstacleMap);
	}

	SECTION("No Path Available")
	{
		Vec2i start = {0, 0};
		Vec2i end = {3, 3}; // Correct unreachable target
		auto path = aStar.findPath(blockedMap, start, end);
		REQUIRE(path.empty());
	}

	SECTION("Start Equals End")
	{
		Vec2i start = {2, 2};
		Vec2i end = {2, 2};
		auto path = aStar.findPath(emptyMap, start, end);
		REQUIRE(path.size() == 1);
		REQUIRE(path.front() == start);
	}

	SECTION("Edge Case: Start or End Outside Map Bounds")
	{
		Vec2i start = {-1, -1};
		Vec2i end = {4, 4};
		auto path = aStar.findPath(emptyMap, start, end);
		REQUIRE(path.empty());

		start = {0, 0};
		end = {10, 10};
		path = aStar.findPath(emptyMap, start, end);
		REQUIRE(path.empty());
	}

	SECTION("Pathfinding with Tight Corridors")
	{
		Vec2i start = {0, 0};
		Vec2i end = {4, 4};
		auto path = aStar.findPath(tightCorridors, start, end);
		verifyPath(path, start, end, tightCorridors);
	}

	SECTION("Pathfinding with Large Open Map")
	{
		std::vector<std::vector<bool>> largeMap(50, std::vector<bool>(50, 0)); // 50x50 open map
		Vec2i start = {0, 0};
		Vec2i end = {49, 49};
		auto path = aStar.findPath(largeMap, start, end);
		verifyPath(path, start, end, largeMap);
	}

	SECTION("Pathfinding with Long and Narrow Corridor")
	{
		std::vector<std::vector<bool>> corridorMap = {
		    {0, 1, 1, 1, 1},
		    {0, 0, 0, 0, 0},
		    {1, 1, 1, 1, 0},
		};
		Vec2i start = {0, 0};
		Vec2i end = {4, 1};
		auto path = aStar.findPath(corridorMap, start, end);
		verifyPath(path, start, end, corridorMap);
	}

	SECTION("Edge Case: Start or End at an Obstacle")
	{
		Vec2i start = {1, 1}; // Start is blocked
		Vec2i end = {4, 4};
		auto path = aStar.findPath(obstacleMap, start, end);
		REQUIRE(path.empty());

		start = {0, 0};
		end = {1, 1}; // End is blocked
		path = aStar.findPath(obstacleMap, start, end);
		REQUIRE(path.empty());
	}
}
