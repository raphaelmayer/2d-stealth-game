#pragma once

#include "../constants.hpp"
#include <vector>

struct Vision {
	float range = 45 * TILE_SIZE; // in pixels
	float angle = 180;           // FOV in degrees
	std::vector<Entity> visibleEnemies = {};
	std::vector<Entity> visibleAllies = {};

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(range, angle, visibleEnemies, visibleAllies);
	}
};