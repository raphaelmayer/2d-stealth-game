#pragma once

#include "../Engine.hpp"
#include "AudioConstants.hpp"
#include <concepts>
#include <filesystem> //should do this somewhere else right?
#include <string>
#include <SDL_mixer.h>
#include "../types.hpp"



class Audio {
  public:
	Audio(int virtualChannels = VIRTUAL_CHANNELS);

	Mix_Music *loadMusicFile(std::string pathToSoundFile);

	Mix_Chunk *loadSoundeffectFile(std::string pathToSoundFile);

	void streamMusic(Mix_Music *loadedSoundFile, int loops, int fadeMs = 0);

	void emit2D(Mix_Chunk *loadedSoundFile, int loops, int fadeMs = 0);

	void emit3D(Mix_Chunk *loadedSoundFile, Vec2f emitterPosition, int listener, int loops, int fadeMs = 0);

	void pauseStream();

	void pauseEmission();

	void pauseAllAudio();

	void resumeStream();

	void resumeEmission();

	void stopEmission();

	void stopStream();

	void stopAllAudio();

	// grouping
};

