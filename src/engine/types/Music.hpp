#pragma once

#include <SDL_mixer.h>
#include <iostream>

class Music {
  public:
	Music() : music_(nullptr) {}; //the nullpointer init is needed aswell and I don´t know exactly why.

	// implementation mostly copied from texture.hpp, irrelevant stuff deleted and kept what I thought I needed
	Music(const char *cStrPath)
	{
		music_ = Mix_LoadMUS(cStrPath);
		if (!music_) {
			std::cout << "Failed to load music file: " << cStrPath << ", SDL_Mixer Error: " << SDL_GetError() << "\n";
		}
	}






	~Music()
	{
		if (music_) {
			Mix_FreeMusic(music_);
		}
	}

	// Non-copyable
	Music(const Music &) = delete;
	Music &operator=(const Music &) = delete;

	// I copied/exchanged the moveable code from the texture type. I want to discuss why it could be needed. Here it fails because it will free a resource that is not there. And then I found out I needed it and now it works xD
	Music(Music &&other) noexcept : music_(other.music_) { other.music_ = nullptr; }

	bool operator!() const { return !music_; }

	Music &operator=(Music &&other) noexcept
	{
		if (this != &other) {
			if (music_) {
				Mix_FreeMusic(music_); 
				                      
			}
			music_ = other.music_;
			other.music_ = nullptr;
		}
		return *this;
	}

	Mix_Music *getMusic() const { return music_; } 

  private:
	Mix_Music *music_; // Internal SDL Mix_Music object
};
