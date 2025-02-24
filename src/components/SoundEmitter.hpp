#pragma once

#include <string>
#include "../engine/types/SoundEffect.hpp"

struct SoundEmitter {
	std::shared_ptr<SoundEffect> soundfile_p;

	//template <class Archive>
	//void serialize(Archive &archive)
	//{
	//	archive(soundfile_p);
	//}
};


