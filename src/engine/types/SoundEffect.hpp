#pragma once

#include <SDL_mixer.h>
#include <iostream>

class SoundEffect {
  public:
	SoundEffect() : soundEffect_(nullptr) {};

	// implementation mostly copied from texture.hpp, irrelevant stuff deleted and kept what I thought I needed
	SoundEffect(const char *cStrPath)
	{
		soundEffect_ = Mix_LoadWAV(cStrPath);
		if (!soundEffect_) {
			std::cout << "Failed to load sound effect file: " << cStrPath << ", SDL_Mixer Error: " << SDL_GetError()
			          << "\n";
		}
	}

	~SoundEffect()
	{
		if (soundEffect_) {
			Mix_FreeChunk(soundEffect_);
		}
	}

	// Non-copyable
	SoundEffect(const SoundEffect &) = delete;
	SoundEffect &operator=(const SoundEffect &) = delete;

	// Movable
	SoundEffect(SoundEffect &&other) noexcept : soundEffect_(other.soundEffect_) { other.soundEffect_ = nullptr; }

	bool operator!() const { return !soundEffect_; }

	SoundEffect &operator=(SoundEffect &&other) noexcept
	{
		if (this != &other) {
			if (soundEffect_) {
				Mix_FreeChunk(soundEffect_); // I need this explained, in the texture implementation you destroy
				                             // texture here
				                             // so I just free the chunk.
			}
			soundEffect_ = other.soundEffect_;
			other.soundEffect_ = nullptr;
		}
		return *this;
	}

	Mix_Chunk *getSoundEffect() const { return soundEffect_; }

  private:
	Mix_Chunk *soundEffect_; // Internal SDL Mix_Chunk object
};
