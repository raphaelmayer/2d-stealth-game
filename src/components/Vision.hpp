#pragma once

#include "../constants.hpp"
#include <easys/easys.hpp>
#include <vector>

struct Vision {
	float range = 20 * TILE_SIZE; // in pixels
	float angle = 180;            // FOV in degrees
	std::vector<Easys::Entity> visibleEnemies = {};
	std::vector<Easys::Entity> visibleAllies = {};

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(range, angle, visibleEnemies, visibleAllies);
	}
};