#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>
#include "Audio.hpp"
#include <array> //lets discuss include clarity and also import std option (c++ 20)



class ChannelManager {
public:
	ChannelManager()
	{
		// started using constexpr and namespaces for this, idea would be 1 engine config file with multiple namespaces
		// that the game developer has to set 1 time.
		for (int index = 0; index < AudioConfig::VIRTUAL_CHANNELS; index++) {
			channelManagementList_[index] = { -1, nullptr, 0, -1 };
		}
	};
	~ChannelManager() {};

	//debating to add this to private but then i cant get it in audio later
	struct ChannelData {
		int emitterID;
		std::shared_ptr<SoundEffect> activeTrack_Ptr;
		int volume;
		int assingedGroup;
	};

	// consitency question: I would probalby name this cleanup. set, get and isplaying will be used in Audio. CLeanup
	// will be placed in the engine as this is where other engine modules update.
	// Audio does not need an update but just for being clean we could put this update into the audio update and run
	// that in the engine?
	void update()
	{
		for (int index = 0; index < channelManagementList_.size(); index++) {
			if (!isChannelPlaying(index)) {
				ChannelData &openedChannelData = channelManagementList_[index];
				openedChannelData = {-1, nullptr, 0, -1};
			}
		}
	}

	void setChannelData(const int channelID, const int emitterID, const std::shared_ptr<SoundEffect> activeTrack_Ptr, const int volume, const int assingedGroup)
	{
		ChannelData nowPlaying = { emitterID, activeTrack_Ptr, volume, assingedGroup };
		channelManagementList_[channelID] = nowPlaying;
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
		return -1; //tbd
	}

  private:

	std::array<ChannelData, AudioConfig::VIRTUAL_CHANNELS> channelManagementList_;

};

//		int counter = 0;
//int channelChosen;
//bool alreadyPlaying = false;
//for (const Entity &entity : entities) {
//
//	if (ecs.hasComponent<SoundEmitter>(entity)) {
//		SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
//		Vec2f &emitterPosition = ecs.getComponent<Positionable>(entity).position;
//		Vec2f listenerPosition = camera_.getPosition() + (Utils::toFloat(engine_.getScreenSize()) / 2);
//
//		for (ChannelData channel : channelManagementList_) {
//			if (channel.emitterID == entity && soundEffect.soundFile_Ptr == channel.activeTrack_Ptr) {
//				alreadyPlaying = true;
//				break;
//			}
//		}
//
//		if (!alreadyPlaying) { // remove 2nd check here, just exists for testing 3d!
//			if (soundEffect.soundFile_Ptr == footStep_Ptr_) {
//				channelChosen = audioDevice_.emit2D(footStep_, {});
//				channelManagementList_[channelChosen] = {entity, footStep_Ptr_, DEFAULT_VOLUME, 0};
//			} else if (soundEffect.soundFile_Ptr == sniperShot_Ptr_) {
//				channelChosen = audioDevice_.emit2D(sniperShot_, {});
//				channelManagementList_[channelChosen] = {entity, sniperShot_Ptr_, DEFAULT_VOLUME, 0};
//			}
//		}

