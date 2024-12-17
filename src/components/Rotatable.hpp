#pragma once

#include "../constants.hpp"

struct Rotatable {
	Rotation rotation;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(rotation);
	}
};