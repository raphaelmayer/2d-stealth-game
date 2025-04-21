#pragma once

#include "../engine/types/Vec2i.hpp"

struct Collider {
	Vec2i size{TILE_SIZE, TILE_SIZE};

	// TODO: do we want to store the following info here? 
	// We could also have a temporary Collision component, which only exists when a collision occurred.
	// Or we could use a std::optional, but this would make the component non-POD.
	bool didCollide = false;
	//Entity lastCollidee;
	Vec2f lastCollisionPosition;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(nullptr);
	}
};
