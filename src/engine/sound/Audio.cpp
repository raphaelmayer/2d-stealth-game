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

int Audio::emit2D(const SoundEffect &loadedSoundFile, const const EmissionOptions &emissionOptions) const
{
	Mix_Chunk *SDL_ChunkType = loadedSoundFile.getSoundEffect(); // we need error handling here.
	if (emissionOptions.fadeMs > 0) {
		return Mix_FadeInChannel(emissionOptions.channelToPlay, SDL_ChunkType, emissionOptions.loops,
		                         emissionOptions.fadeMs);
	} else {
		return Mix_PlayChannel(emissionOptions.channelToPlay, SDL_ChunkType, emissionOptions.loops);
	}
}

int Audio::emit3D(const SoundEffect &loadedSoundFile, const Vec2f &emitterPosition, const Vec2f &listenerPosition,
                  const EmissionOptions &emissionOptions) const
{
	int channelChosen; // TODO return emit2D directly not with this variable
	if (!Mix_Playing(30)) {
		channelChosen = emit2D(loadedSoundFile, emissionOptions);
	}
	Sint16 angle = Audio::calculateAudioAngle(emitterPosition, listenerPosition);
	int distance = Audio::calculateAudioDistance(emitterPosition, listenerPosition);
	if (distance <= 255) { // normalize distance
		if (getVolume(30) == 0) {
			setVolume(30, DEFAULT_VOLUME);
		}
		Mix_SetPosition(30, angle, static_cast<Uint8>(distance));
	} else {
		setVolume(30, 0);
	}
	return channelChosen;
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

void Audio::resumeAllAudio() const 
{
	Mix_Resume(-1);
	Mix_ResumeMusic();
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

void Audio::stopStream() const
{
	Mix_HaltMusic();
}

void Audio::stopAllAudio() const
{
	Mix_HaltMusic();
	Mix_HaltChannel(-1);
}

void Audio::setVolume(const int &channel, const int &volume) const
{
	Mix_Volume(channel, volume);
}

void Audio::setAllVolume(const int& volume) const 
{
	Mix_Volume(-1, volume);
	Mix_VolumeMusic(volume);
}

int Audio::getVolume(const int &channel) const
{
	return Mix_Volume(channel, -1);
}

//int Audio::getStreamVolume() const 
//{
//	return Mix_GetMusicVolume();
//}

Sint16 Audio::calculateAudioAngle(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const
{
	float adjacent = emitterPosition.x - listenerPosition.x;
	float opposite = emitterPosition.y - listenerPosition.y;
	float angleInRadians = std::atan2(opposite, adjacent);
	float angleInDegrees = angleInRadians * 180 / std::numbers::pi;
	return angleInDegrees;
}
int Audio::calculateAudioDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const
{
	float distance = (listenerPosition - emitterPosition).length() * 2;

	return static_cast<int>(distance);
}

bool Audio::isChannelPlaying(const int channelId)
    const // TODO not working function as -1 returns number of channels playing --> Update: actually since we want to
          // disincourage the -1 of SDL_Mixer I would argue that this is working
{
	if (Mix_Playing(channelId) == 0) {
		return false;
	} else {
		return true;
	}
}
