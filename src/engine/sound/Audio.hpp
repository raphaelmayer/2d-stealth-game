#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "ChannelManager.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>

//  Group all functions according to their object

class Audio {
  public:
	Audio();
	~Audio();

	struct EmissionOptions {  //can´t be private, we expect user to give these
		int loops = 0;
		int fadeMs = 0;
		int distance_modifier = 1; // a way to let game developer influence distance calculation
	};

	//audios update function meant to run in the engine. currently only used for reseting channels in the channel manager
	void update();

	// loads file from specified path in proprietary format. please use .mp3 or .ogg files only
	Music loadMusicFile(const std::string &pathToSoundFile) const;

	void streamMusic(const Music &loadedSoundFile, int loops, int fadeMs = 0) const;

	//pauses the music stream (timestamp kept)
	void pauseStream() const;

	// stops the music stream (soundfile thrown)
	void stopStream() const;

	//resume stream from paused position
	void resumeStream() const;

	// IN SDL_Mixer we can query the volume of the currently playing music object but not the stream channel, stupido
	// int getStreamVolume() const;

	//loads file from specified path in proprietary format. only accepts WAV
	SoundEffect loadSoundEffectFile(const std::string &pathToSoundFile) const;

	// plays specified SoundEffect file on any free channel
	int emit2D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr, const EmissionOptions &emissionOptions);

	// plays specified SoundEffect file on any free channel using a simple 3d spacialization effect, use the distance modifier
	// emission option to influence cut off (higher modifier decreases distance for cut off. i.e. 2 = half the distance)
	int emit3D(const int &emitterID, const std::shared_ptr<SoundEffect> &soundEffect_Ptr, const Vec2f &emitterPosition,
	           const Vec2f &listenerPosition,
	           const EmissionOptions &emissionOptions);

	// pauses emission (timestamp kept) on specified, or all channels
	void pauseEmission(const int channelToPause = -1) const;

	// stops emission (soundfile thrown) on specified channel, or all channels
	void stopEmission(int channelToStop = -1);

	// resumes emission from paused position on specified channel, or all channels
	void resumeEmission(const int channelToResume = -1) const;

	// sets Volume on specified channels, leave empty for all channels, maximum volume is 128
	void setVolume(const int &volume, const int &channel = -1) const;
	
	//get the current volume of a channel
	int getVolume(const int &channel) const;

	//pauses all audio (timestamp kept)
	void pauseAllAudio() const;

	//stops all audio (soundfile thrown)
	void stopAllAudio() const;

	//resumes emission on all channels and the stream
	void resumeAllAudio() const; 

	// maximum volume is 128, sets Volume for stream and all channels
	void setAllVolume(const int &volume) const;

  private:
	Sint16 calculateAudioAngle(const Vec2f &emitterPosition,
	                           const Vec2f &listenerPosition) const;
	int calculateAudioDistance(const Vec2f &emitterPosition, const Vec2f &listenerPosition, const int distance_modifier) const;

	ChannelManager channelManager_;
};