#include "Audio.hpp"
#include <cmath>
#include <numbers>

Audio::Audio(int virtualChannels)
{

	// I will start wrapping SDL_mixer here. I want to discuss where to place what as I am still unsure but that should
	// not deter me from starting the work now

	SDL_Init(SDL_INIT_AUDIO);
	if (Mix_OpenAudioDevice(FREQUENCY, MIXER_FORMAT, STEREO, AUDIO_DEVICE_CHUNK_SIZE, NULL,
	                        SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE)
	    < 0)
		fprintf(stderr, "Audio initialization failed: %s\n", Mix_GetError());
	Mix_AllocateChannels(virtualChannels);
}

Audio::~Audio()
{
	Mix_CloseAudio();
	Mix_Quit(); // if we need Mix_Init(), we also need to call this
}

Music Audio::loadMusicFile(const std::string &pathToSoundFile) const // make a type for music and chunks
{
	const char *cStrPath = pathToSoundFile.c_str();
	return Music(cStrPath);
}

SoundEffect Audio::loadSoundEffectFile(const std::string &pathToSoundFile) const // please rate my constant consting
{
	const char *cStrPath = pathToSoundFile.c_str();
	return SoundEffect(cStrPath);
}

void Audio::streamMusic(const Music &loadedSoundFile, int loops, int fadeMs) const
{
	Mix_Music *SDL_MusicType = loadedSoundFile.getMusic();
	if (fadeMs > 0) {
		Mix_FadeInMusic(SDL_MusicType, loops, fadeMs);

	} else {
		Mix_PlayMusic(SDL_MusicType, loops);
	}
}

int Audio::emit2D(const int channelToPlay, const SoundEffect &loadedSoundFile, const int loops,
                  const int fadeMs) const // should prolly return the used channel like the mixer functions to get the
                                          // functionality needed for channelmanager
{
	int chosenChannel;
	Mix_Chunk *SDL_ChunkType = loadedSoundFile.getSoundEffect(); // we need error handling here.
	if (fadeMs > 0) {
		chosenChannel = Mix_FadeInChannel(channelToPlay, SDL_ChunkType, loops, fadeMs);
	} else {
		chosenChannel = Mix_PlayChannel(channelToPlay, SDL_ChunkType, loops);
	}
	return chosenChannel;
}

void Audio::emit3D(const SoundEffect &loadedSoundFile, const Vec2f &emitterPosition, const Vec2f &listenerPosition,
                   const int loops, const int fadeMs) const // test function
{
	if (Mix_Playing(30) == 0) {
		emit2D(30, loadedSoundFile, -1, 0);
	}
	Sint16 angle = Audio::calculateAngle(emitterPosition, listenerPosition);
	int distance = Audio::calculateDistance(emitterPosition, listenerPosition);
	if (distance <= 255) {
		Mix_SetPosition(30, angle, static_cast<Uint8>(distance));
	} else {
		Mix_Volume(30, 0);
	}
}

void Audio::pauseStream() const
{
	Mix_PauseMusic();
}

void Audio::pauseEmission(const int channelToPause) const
{
	Mix_Pause(channelToPause);
}

void Audio::pauseAllAudio() const // newer function Mix_PauseAudio available in Mixer 2.8.0
{
	Mix_Pause(-1);
}

void Audio::resumeStream() const
{
	Mix_ResumeMusic();
}

void Audio::resumeEmission(const int channelToResume) const
{
	Mix_Resume(channelToResume);
}

void Audio::stopEmission(int channelToStop)
{
	Mix_HaltChannel(channelToStop);
}

void Audio::stopStream()
{
	Mix_HaltMusic();
}

void Audio::stopAllAudio()
{
	Mix_HaltMusic();
	Mix_HaltChannel(-1);
}

void Audio::manageVolume(const int &channel, const int &volume) const
{
	Mix_Volume(channel, volume);
}

Sint16 Audio::calculateAngle(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const
{
	float adjacent = emitterPosition.x - listenerPosition.x;
	float opposite = emitterPosition.y - listenerPosition.y;
	float angleInRadians = std::atan2(opposite, adjacent);
	float angleInDegrees = angleInRadians * 180 / std::numbers::pi;
	return angleInDegrees;
}
int Audio::calculateDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const
{
	float adjacent = fabs(emitterPosition.x - listenerPosition.x);
	float opposite = fabs(emitterPosition.y - listenerPosition.y);
	float hypotenuse;
	if (adjacent == 0.00f) {
		hypotenuse = opposite;
	} else if (opposite == 0.00f) {
		hypotenuse = adjacent;
	} else {
		hypotenuse = std::sqrt(adjacent * adjacent + opposite * opposite);
	}
	float distance = hypotenuse * 2;
	return static_cast<int>(distance);
}

bool Audio::isChannelPlaying(int channelId) const 
{
	if (Mix_Playing(channelId) == 0) {
		return false;
	} else {
		return true;
	}
}
