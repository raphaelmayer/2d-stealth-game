#include "Audio.hpp"
#include <cmath>
#include <numbers>

Audio::Audio() : channelManager_()
{

	// I will start wrapping SDL_mixer here. I want to discuss where to place what as I am still unsure but that should
	// not deter me from starting the work now

	SDL_Init(SDL_INIT_AUDIO);
	if (Mix_OpenAudioDevice(AudioConfig::FREQUENCY, AudioConfig::MIXER_FORMAT, AudioConfig::STEREO,
	                        AudioConfig::AUDIO_DEVICE_CHUNK_SIZE, NULL,
	                        SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE)
	    < 0)
		fprintf(stderr, "Audio initialization failed: %s\n", Mix_GetError());
	Mix_AllocateChannels(AudioConfig::VIRTUAL_CHANNELS);
}

Audio::~Audio()
{
	Mix_CloseAudio();
	Mix_Quit(); // if we need Mix_Init(), we also need to call this
}

void Audio::update()
{
	channelManager_.resetChannels();
}

Music Audio::loadMusicFile(const std::string &pathToSoundFile) const // make a type for music and chunks
{
	const char *cStrPath = pathToSoundFile.c_str();
	return Music(cStrPath);
}

SoundEffect Audio::loadSoundEffectFile(const std::string &pathToSoundFile) const 
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

int Audio::emit2D(
    const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr,
    const EmissionOptions &emissionOptions) // should prolly be const and setting of channel data happen somewhere else
{
	int channelChosen;
	if (!channelManager_.isEmitterPlayingThis(emitterID, soundEffect_Ptr)) {
		SoundEffect& soundEffect = *soundEffect_Ptr; 
		Mix_Chunk* SDL_ChunkType = soundEffect.getSoundEffect(); 
		if (emissionOptions.fadeMs > 0) {
			channelChosen = Mix_FadeInChannel(-1, SDL_ChunkType, emissionOptions.loops,
			                         emissionOptions.fadeMs);
		} else {
			channelChosen = Mix_PlayChannel(-1, SDL_ChunkType, emissionOptions.loops);
		}
		channelManager_.setChannelData(channelChosen, emitterID, soundEffect_Ptr, AudioConfig::DEFAULT_VOLUME, -1); 
	}
	return channelManager_.whereIsEmitterPlayingThis(emitterID, soundEffect_Ptr);
}

int Audio::emit3D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr,
                  const Vec2f &emitterPosition, const Vec2f &listenerPosition,
                  const EmissionOptions &emissionOptions)
{
	int channelChosen; // TODO return emit2D directly not with this variable -> I cannot?!
	if (!channelManager_.isEmitterPlayingThis(emitterID, soundEffect_Ptr)) 
	{
		channelChosen = emit2D(emitterID, soundEffect_Ptr, emissionOptions);
	} else {
		channelChosen = channelManager_.whereIsEmitterPlayingThis(emitterID, soundEffect_Ptr);
	}
	Sint16 angle = calculateAudioAngle(emitterPosition, listenerPosition);
	int distance = calculateAudioDistance(emitterPosition, listenerPosition);
	if (distance <= 255) { // normalize distance --> why though, I have to feed it into setposition here anyhow
		if (getVolume(channelChosen) == 0) {
			setVolume(channelChosen, AudioConfig::DEFAULT_VOLUME);
		}
		Mix_SetPosition(channelChosen, angle, static_cast<Uint8>(distance));
	} else {
		setVolume(channelChosen, 0);
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

void Audio::setVolume(const int &volume, const int &channel) const
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


