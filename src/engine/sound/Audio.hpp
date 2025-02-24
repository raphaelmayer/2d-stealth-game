#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "ChannelManager.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>


class Audio {
  public:
	Audio(int virtualChannels = VIRTUAL_CHANNELS);
	~Audio();

	Music loadMusicFile(const std::string &pathToSoundFile) const;

	SoundEffect loadSoundEffectFile(const std::string &pathToSoundFile) const;

	void streamMusic(const Music &loadedSoundFile, int loops, int fadeMs = 0) const;
	
	// plays specified file on specified channel, -1 for all channels
	int emit2D(const int channelToPlay, const SoundEffect &loadedSoundFile, const int loops,
	           const int fadeMs = 0) const;

	void emit3D(const SoundEffect &loadedSoundFile, const Vec2f &emitterPosition, const Vec2f &listenerPosition,
	            const int loops, const int fadeMs = 0) const;

	void pauseStream() const;

	// pauses emission on specified channel, -1 for all channels
	void pauseEmission(const int channelToPause) const;

	void pauseAllAudio() const;

	void resumeStream() const;

	// resumes emission on specified channel, -1 for all channels
	void resumeEmission(const int channelToResume) const;

	// stops emission on specified channel, -1 for all channels
	void stopEmission(int channelToStop);

	void stopStream();

	void stopAllAudio();

	// maximum volume is 128, -1 for all channels
	void manageVolume(const int &channel, const int &volume) const;

	bool isChannelPlaying(int channelId) const;

	// grouping not touched for, will be implemented when/if deemed necessary

  private:
	Sint16 calculateAngle(const Vec2f &emitterPosition,
	                      const Vec2f &listenerPosition) const; // let´s talk about const before body again please
	int calculateDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const;
};
