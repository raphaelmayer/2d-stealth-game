#pragma once

#include"../constants.hpp"
#include <vector>

struct Vision {
	int range = 5 * TILE_SIZE;						// in pixels
	int angle = 180;								// FOV in degrees
	std::vector<Entity> visibleEntities = {};

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(range, visibleEntities);
	}
};