#pragma once

#include <vector>

class SoundSource; // TODO

struct Hearing {
	int hearingRange = 50;
	std::vector<SoundSource> heardSounds = {};

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(hearingRange, heardSounds);
	}
};