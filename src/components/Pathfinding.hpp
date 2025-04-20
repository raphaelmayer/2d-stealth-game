#pragma once

#include "../engine/types/Vec2i.hpp"
#include <vector>

// TODO: We could make this a temporary component.

// Entities with this component are picked up by the PathfindingSystem.
// They receive a targetPosition as input and can move towards them on the grid.
struct Pathfinding {
	Vec2i targetPosition{-1, -1}; // Current target
	std::vector<Vec2i> path = {}; // Current path to the target
	int pathIndex = 0;
};