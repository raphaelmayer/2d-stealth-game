#pragma once

#include "../types.hpp"
#include "AudioConstants.hpp"
#include "Listener.hpp"
#include <SDL_mixer.h>
#include <concepts>
#include <filesystem>
#include <string>
#include <array>
//lets discuss include clarity and also import std option (c++ 20)



class ChannelManager {
  public:
	ChannelManager(int virtualChannels) 
	{ 
		
		
	};
	~ChannelManager() {};
	

  private:

	enum class ChannelGroup {
		
	};
	struct ChannelData {
		int emitterID;
		int soundfileID;
		int volume;
		ChannelGroup assingedGroup;
	};
	std::array<ChannelData, VIRTUAL_CHANNELS> ChannelManagementList_;
	
	
};
