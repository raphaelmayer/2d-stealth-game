#pragma once

#include <easys/easys.hpp>
#include <vector>

struct Inventory {
	std::vector<Easys::Entity> items;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(items);
	}
};
