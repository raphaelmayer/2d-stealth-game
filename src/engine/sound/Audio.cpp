#include "Audio.hpp"


Audio::Audio(int virtualChannels)
{

	// I will start wrapping SDL_mixer here. I want to discuss where to place what as I am still unsure but that should
	// not deter me from starting the work now

	SDL_Init(SDL_INIT_AUDIO); // currently in engine.cpp
	if (Mix_OpenAudioDevice(FREQUENCY, MIXER_FORMAT, STEREO, AUDIO_DEVICE_CHUNK_SIZE, NULL,
	                        SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE)
	    < 0)
		fprintf(stderr, "Audio initialization failed: %s\n", Mix_GetError());
	Mix_AllocateChannels(virtualChannels);
}

Mix_Music *Audio::loadMusicFile(std::string pathToSoundFile) // make a type for music and chunks
{
	const char *cStrPath = pathToSoundFile.c_str();
	return Mix_LoadMUS(cStrPath);
}

Mix_Chunk *Audio::loadSoundeffectFile(std::string pathToSoundFile)
{
	const char *cStrPath = pathToSoundFile.c_str();
	return Mix_LoadWAV(cStrPath);
}

void Audio::streamMusic(Mix_Music *loadedSoundFile, int loops, int fadeMs)
{
	if (fadeMs > 0) {
		Mix_FadeInMusic(loadedSoundFile, loops, fadeMs);
	} else {
		Mix_PlayMusic(loadedSoundFile, loops);
	}
}

void Audio::emit2D(Mix_Chunk *loadedSoundFile, int loops, int fadeMs)
{
	if (fadeMs > 0) {
		Mix_FadeInChannel(-1, loadedSoundFile, loops, fadeMs);
	} else {
		Mix_PlayChannel(-1, loadedSoundFile, loops);
	}
}

void Audio::pauseStream()
{
	Mix_PauseMusic();
} // I can´t really think of a wrap here changing anything about the function

void Audio::pauseEmission() {}

void Audio::pauseAllAudio() // newer function Mix_PauseAudio available in Mixer 2.8.0
{
	Mix_Pause(-1);
}

void Audio::resumeStream()
{
	Mix_ResumeMusic();
}
