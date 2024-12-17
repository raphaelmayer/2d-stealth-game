#pragma once

struct Collider {
	// no need for any params, since if collider on some tile, this single tile is occupied

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(nullptr);
	}
};
