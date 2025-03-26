#pragma once

#include "../engine/types/Vec2i.hpp"

struct Collider {
	Vec2i size{TILE_SIZE, TILE_SIZE};

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(nullptr);
	}
};
