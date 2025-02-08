#pragma once


#define FREQUENCY 44100
#define MIXER_FORMAT AUDIO_S16SYS       // if we can make sure every file comes in the correct format we can gain performance here, S16 equates
	                                    // to 16bit-Audio
#define STEREO 2                        // for mono use 1, dont want to use Channels here because it can get mixed up
#define AUDIO_DEVICE_CHUNK_SIZE 1024    // save default is 2048, 1024-4096 is range that should be tried, lower equates to lower latency but risks
	                                    // dropouts
#define VIRTUAL_CHANNELS 32             // channels is sadly a misleading word here. SDL_Mixers fault. This defines how many sounds can play at the same
	                                    // time. 32 is a good default. Errors/warnings to be designed so that increasing the number of channels is given
	                                    // as an option
