#pragma once

struct Tombstone {
	/* data */

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(nullptr);
	}
};
