#pragma once

#include "../ecs/Entity.hpp"
#include <vector>

struct Inventory {
	std::vector<Entity> items;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(items);
	}
};
