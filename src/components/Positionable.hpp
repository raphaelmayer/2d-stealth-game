#pragma once

#include "../engine/Vec2d.hpp"

// Represents the position on the map in pixels
struct Positionable {
	Vec2d position;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(position);
	}
};
