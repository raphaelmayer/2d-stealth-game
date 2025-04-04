#include "Audio.hpp"
#include <cmath>
#include <numbers>

Audio::Audio() : channelManager_()
{
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

void Audio::streamMusic(const Music &loadedSoundFile, int loops, int fadeMs) const
{
	Mix_Music *SDL_MusicType = loadedSoundFile.getMusic();
	if (fadeMs > 0) {
		Mix_FadeInMusic(SDL_MusicType, loops, fadeMs);
	} else {
		Mix_PlayMusic(SDL_MusicType, loops);
	}
}

void Audio::pauseStream() const
{
	Mix_PauseMusic();
}

void Audio::stopStream() const
{
	Mix_HaltMusic();
}

void Audio::resumeStream() const
{
	Mix_ResumeMusic();
}

// below function could work if channel manager gets a 33rd entry for the music that is playing then we can query this
// like a channel. int Audio::getStreamVolume() const
//{
//	return Mix_GetMusicVolume();
// }

SoundEffect Audio::loadSoundEffectFile(const std::string &pathToSoundFile) const
{
	const char *cStrPath = pathToSoundFile.c_str();
	return SoundEffect(cStrPath);
}

int Audio::playSoundEffect(const std::shared_ptr<SoundEffect> soundEffect_Ptr, const EmissionOptions &emissionOptions) const
{
	Mix_Chunk *SDL_ChunkType = soundEffect_Ptr->getSoundEffect();
	if (emissionOptions.fadeMs > 0) {
		return Mix_FadeInChannel(AudioConfig::ANY_CHANNEL, SDL_ChunkType, emissionOptions.loops,
		                         emissionOptions.fadeMs);
	} else {
		return Mix_PlayChannel(AudioConfig::ANY_CHANNEL, SDL_ChunkType, emissionOptions.loops);
	}
}

int Audio::emit2D(
    const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr,
    const EmissionOptions &emissionOptions) // should prolly be const and setting of channel data happen somewhere else
{
	if (!channelManager_.isEmitterPlayingThis(emitterID, soundEffect_Ptr)) {
		Mix_Chunk *SDL_ChunkType = soundEffect_Ptr->getSoundEffect();

		int channelChosen = playSoundEffect(soundEffect_Ptr, emissionOptions);

		channelManager_.setChannelData(channelChosen, emitterID, soundEffect_Ptr, AudioConfig::DEFAULT_VOLUME,
		                               -1); 
	}
	return channelManager_.whereIsEmitterPlayingThis(emitterID, soundEffect_Ptr);
}

int Audio::applySpatialization(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr,
                                const Vec2f &emitterPosition, const Vec2f &listenerPosition,
                                const EmissionOptions &emissionOptions) const{

    int angle = calculateAudioAngle(emitterPosition, listenerPosition);
	int distance = calculateAudioDistance(emitterPosition, listenerPosition, emissionOptions.distance_modifier);
	int channel = channelManager_.whereIsEmitterPlayingThis(emitterID, soundEffect_Ptr);
	if (distance <= 255) { // normalize distance --> why though, I have to feed it into setposition here anyhow
		if (getVolume(channel) == 0) {
			setVolume(AudioConfig::DEFAULT_VOLUME, channel);
		}
		Mix_SetPosition(channel, static_cast<Sint16>(angle), static_cast<Uint8>(distance));
	} else {
		setVolume(channel, 0);
	}
	return channel;
}


int Audio::emit3D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr,
                  const Vec2f &emitterPosition, const Vec2f &listenerPosition, const EmissionOptions &emissionOptions)
{
	int channelChosen; // TODO return emit2D directly not with this variable -> I cannot?!
	if (!channelManager_.isEmitterPlayingThis(emitterID, soundEffect_Ptr)) {
		channelChosen = emit2D(emitterID, soundEffect_Ptr, emissionOptions);
	} else {
		channelChosen = channelManager_.whereIsEmitterPlayingThis(emitterID, soundEffect_Ptr);
	}
	applySpatialization(emitterID, soundEffect_Ptr, emitterPosition, listenerPosition, emissionOptions);

	return channelChosen;
}



void Audio::pauseEmission(const int channelToPause) const
{
	Mix_Pause(channelToPause);
}

void Audio::stopEmission(int channelToStop)
{
	Mix_HaltChannel(channelToStop);
}

void Audio::resumeEmission(const int channelToResume) const
{
	Mix_Resume(channelToResume);
}

void Audio::setAllVolume(const int &volume) const
{
	Mix_Volume(AudioConfig::ANY_CHANNEL, volume);
	Mix_VolumeMusic(volume);
}

int Audio::getVolume(const int &channel) const
{
	return Mix_Volume(channel, -1);
}

void Audio::pauseAllAudio() const // newer function Mix_PauseAudio available in Mixer 2.8.0
{
	Mix_Pause(AudioConfig::ANY_CHANNEL);
}

void Audio::stopAllAudio() const
{
	Mix_HaltMusic();
	Mix_HaltChannel(AudioConfig::ANY_CHANNEL);
}

void Audio::resumeAllAudio() const
{
	Mix_Resume(AudioConfig::ANY_CHANNEL);
	Mix_ResumeMusic();
}

void Audio::setVolume(const int &volume, const int &channel) const
{
	Mix_Volume(channel, volume);
}

int Audio::calculateAudioAngle(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const
{
	float adjacent = emitterPosition.x - listenerPosition.x;
	float opposite = emitterPosition.y - listenerPosition.y;
	float angleInRadians = std::atan2(opposite, adjacent);
	float angleInDegrees = angleInRadians * 180 / std::numbers::pi;
	return static_cast<int>(angleInDegrees);
}

int Audio::calculateAudioDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition,
                                  const int distance_modifier) const
{
	float distance = (listenerPosition - emitterPosition).length() * distance_modifier;

	return static_cast<int>(distance);
}

const ChannelManager &Audio::getChannelManager() const
{
	return channelManager_;
}