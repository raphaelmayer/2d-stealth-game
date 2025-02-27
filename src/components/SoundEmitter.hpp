#pragma once

#include <string>
#include "../engine/types/SoundEffect.hpp"

struct SoundEmitter {
	std::shared_ptr<SoundEffect> soundFile_Ptr;

	//template <class Archive>
	//void serialize(Archive &archive)
	//{
	//	archive(soundFile_Ptr);
	//}
};


