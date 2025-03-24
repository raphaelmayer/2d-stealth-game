#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>
#include "Audio.hpp"
#include <array> 

class ChannelManager {
public:
	ChannelManager()
	{
		// started using constexpr and namespaces for this, idea would be 1 engine config file with multiple namespaces
		// that the game developer has to set 1 time.
		for (int index = 0; index < AudioConfig::VIRTUAL_CHANNELS; index++) {
			channelManagementList_[index] = { -1, nullptr, 0, -1 };
		}
		channelManager_ = this;
	};
	~ChannelManager() 
	{
		if (channelManager_ == this) {
			channelManager_ = nullptr;
		}
	};

	//debating to add this to private but then i cant get it in audio later
	struct ChannelData {
		int emitterID;
		std::shared_ptr<SoundEffect> activeTrack_Ptr;
		int volume;
		int assingedGroup; //TODO --> necessary functions to get groups
	};

	void resetChannel(const int &channelID)
	{
		if (!isChannelPlaying(channelID)) {
			ChannelData &openedChannelData = channelManagementList_[channelID];
			openedChannelData = {-1, nullptr, 0, -1};
		}		
	}

	void resetChannels() 
	{
		for (int index = 0; index < channelManagementList_.size(); index++) {
			if (!isChannelPlaying(index)) {
				ChannelData &openedChannelData = channelManagementList_[index];
				openedChannelData = {-1, nullptr, 0, -1};
			}
		}
	}

	void setChannelData(const int channelID, const int emitterID, const std::shared_ptr<SoundEffect> activeTrack_Ptr,
	                    const int volume, const int assingedGroup)
	{
		if (channelID != AudioConfig::ANY_CHANNEL) {
			ChannelData nowPlaying = {emitterID, activeTrack_Ptr, volume, assingedGroup};
			channelManagementList_[channelID] = nowPlaying;
			setCallback(channelID);
		}
		
	}

	ChannelData getChannelData(const int channelID) const { return channelManagementList_[channelID]; }

	bool isChannelPlaying(const int channelID) const
	{
		if (Mix_Playing(channelID) == 0) {
			return false;
		} else {
			return true;
		}
	}

	bool isEmitterPlaying(const int emitterID)  const //don´t know if this check will be used
	{
		for (ChannelData openChannel : channelManagementList_) {
			if (emitterID == openChannel.emitterID) {
				return true;
			}
		}
		return false;
	}

	bool isEmitterPlayingThis(const int emitterID, const std::shared_ptr<SoundEffect> &trackPtr) const //custom return type to get true and the channel?
	{
		for (ChannelData openChannel : channelManagementList_) {
			if (emitterID == openChannel.emitterID && trackPtr == openChannel.activeTrack_Ptr) {
				return true;
			}
		}
		return false;
	}

	int whereIsEmitterPlayingThis(const int emitterID, const std::shared_ptr<SoundEffect>& trackPtr) const
	{
		for (int index = 0; index < channelManagementList_.size(); index++) {
			const ChannelData &openedChannelData = channelManagementList_[index];
			if (emitterID == openedChannelData.emitterID && trackPtr == openedChannelData.activeTrack_Ptr) 
			{
				return index;
			}
		}
		return -1; //we could think about custom type
	}

  private:

	static ChannelManager* channelManager_;

	void setCallback(const int& channelID)
	{
		Mix_ChannelFinished(Callback);
	}

	// either outside of class or a static member function. Problem then is the resetChannel method. -> check fix above (is the singleton approach correct?)
	static void Callback(int channelID)
	{
		channelManager_->resetChannel(channelID);
	}
	
	std::array<ChannelData, AudioConfig::VIRTUAL_CHANNELS> channelManagementList_;

};