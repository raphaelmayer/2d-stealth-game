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
	Audio();
	~Audio();

	struct EmissionOptions {  //can´t be private, we expect user to give these
		int loops = 0;
		int fadeMs = 0;
	};

	void update();

	Music loadMusicFile(const std::string &pathToSoundFile) const;

	SoundEffect loadSoundEffectFile(const std::string &pathToSoundFile) const;

	void streamMusic(const Music &loadedSoundFile, int loops, int fadeMs = 0) const;

	// plays specified file on any channel, or a specified one
	int emit2D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr, const EmissionOptions &emissionOptions);

	int emit3D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr, const Vec2f &emitterPosition,
	           const Vec2f &listenerPosition,
	           const EmissionOptions &emissionOptions);

	void pauseStream() const;

	// pauses emission on specified, or all channels
	void pauseEmission(const int channelToPause = -1) const;

	void pauseAllAudio() const;

	void resumeStream() const;

	// resumes emission on specified channel, or all channels
	void resumeEmission(const int channelToResume = -1) const;

	//resumes emission on all channels and the stream
	void resumeAllAudio() const; 

	// stops emission on specified channel, -1 for all channels
	void stopEmission(int channelToStop);

	//stops the music stream
	void stopStream() const;

	// difference between stop and pause
	void stopAllAudio() const;

	// sets Volume on specified channels, leave empty for all channels, maximum volume is 128
	void setVolume(const int &volume, const int &channel = -1) const;
	
	// maximum volume is 128, sets Volume for stream and all channels
	void setAllVolume(const int &volume) const;

	int getVolume(const int &channel) const;

	// IN SDL_Mixer we can query the volume of the currently playing music object but not the stream channel, stupido
	//int getStreamVolume() const;

	
	bool isChannelPlaying(const int channelId) const; //remove this from api as handled by channel manager

	// grouping not touched for, will be implemented when/if deemed necessary

  private:
	Sint16 calculateAudioAngle(const Vec2f &emitterPosition,
	                           const Vec2f &listenerPosition) const;
	int calculateAudioDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition) const;

	ChannelManager channelManager_;
};