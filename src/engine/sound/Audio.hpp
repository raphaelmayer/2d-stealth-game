#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "ChannelManager.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>

// TODO: remove int -1 returns to modernize
//  Group all functions according to their object
// Channel Management to engine
//
//
//

class Audio {
  public:
	Audio(int virtualChannels = VIRTUAL_CHANNELS);
	~Audio();

	struct EmissionOptions { // should move to private
		int channelToPlay = -1;
		int loops = 0;
		int fadeMs = 0;
	};

	Music loadMusicFile(const std::string &pathToSoundFile) const;

	SoundEffect loadSoundEffectFile(const std::string &pathToSoundFile) const;

	void streamMusic(const Music &loadedSoundFile, int loops, int fadeMs = 0) const;

	// plays specified file on any channel, or a specified one
	int emit2D(const SoundEffect &loadedSoundFile, const EmissionOptions &emissionOptions) const;

	int emit3D(const SoundEffect &loadedSoundFile, const Vec2f &emitterPosition, const Vec2f &listenerPosition,
	           const EmissionOptions &emissionOptions) const;

	void pauseStream() const;

	// pauses emission on specified channel, -1 for all channels
	void pauseEmission(const int channelToPause) const;

	void pauseAllAudio() const;

	void resumeStream() const;

	// resumes emission on specified channel, -1 for all channels
	void resumeEmission(const int channelToResume) const;

	void resumeAll() const; // TODO

	// stops emission on specified channel, -1 for all channels
	void stopEmission(int channelToStop);

	void stopStream() const;

	// difference between stop and pause
	void stopAllAudio() const;

	// maximum volume is 128, -1 for all channels
	void setVolume(const int &channel, const int &volume) const;

	int getVolume(const int &channel) const;

	bool isChannelPlaying(const int channelId) const;

	// grouping not touched for, will be implemented when/if deemed necessary

  private:
	Sint16 calculateAudioAngle(const Vec2f &emitterPosition,
	                           const Vec2f &listenerPosition) const; // let´s talk about const before body again please
	int calculateAudioDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const;
};
