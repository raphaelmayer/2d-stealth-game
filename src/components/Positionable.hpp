#pragma once

#include "../engine/types/Vec2f.hpp"

// Represents the position on the map in pixels
struct Positionable {
	Vec2f position;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(position);
	}
};

//struct NewPos : public Vec2i {
//	template <class Archive>
//	void serialize(Archive &archive)
//	{
//		archive(x,y);
//	}
//};
