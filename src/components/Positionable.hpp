#pragma once

#include "../engine/types/Vec2i.hpp"

// Represents the position on the map in pixels
struct Positionable {
	Vec2i position;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(position);
	}
};
