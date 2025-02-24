#include "../components/SoundEmitter.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"
#include <SDL.h>
#include <stdint.h>
#include <memory>

class AudioSystem final : public System {
  public:
	explicit AudioSystem(const Engine &engine) : engine_(engine)
	{
		footStep_ = audioDevice_.loadSoundEffectFile(SFX_FOOTSTEP);
		sniperShot_ = audioDevice_.loadSoundEffectFile(SFX_SNIPER_SHOT_AND_RELOAD);

		audioDevice_.manageVolume(-1, VOLUME_STANDARD);

		// Mix_ChannelFinished(channelFinished);
	};

	void update(ECSManager &ecs, const double deltaTime) override
	{
		// Start Background Music at Start of Game loop
		if (!backgroundMusic_) {
			backgroundMusic_ = audioDevice_.loadMusicFile(BACKGROUND_MUSIC_1);
			audioDevice_.streamMusic(backgroundMusic_, -1);
		}

		// testing to check for isMoving here or not could work well
		const std::set<Entity> &entities = ecs.getEntities();
		for (Entity entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity)) {
				RigidBody &rigidBody = ecs.getComponent<RigidBody>(entity);
				if (rigidBody.isMoving) {
					if (!ecs.hasComponent<SoundEmitter>(entity)) {
						ecs.addComponent<SoundEmitter>(entity, {footStep_ptr_});
					}
				}
				if (rigidBody.isShooting) {
					if (!ecs.hasComponent<SoundEmitter>(entity)) {
						ecs.addComponent<SoundEmitter>(entity, {sniperShot_ptr_});
					}
				}
			}
		}

		// Play Soundeffects
		/* for (Entity entity : entities) {
		    if (entity == PLAYER) {
		        RigidBody &rigidBody = ecs.getComponent<RigidBody>(entity);
		        if (rigidBody.isMoving ) {
		            if (!ecs.hasComponent<SoundEmitter>(entity)) {
		                ecs.addComponent<SoundEmitter>(entity, {"footstep"});
		            }

		        }
		    }
		}*/
		int counter = 0;
		int channelChosen;
		bool alreadyPlaying = false;
		for (const Entity &entity : entities) {
			
			if (ecs.hasComponent<SoundEmitter>(entity)) {
				SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
				Vec2f &position = ecs.getComponent<Positionable>(entity).position;
				int pos_x = static_cast<int>(position.x);
				int pos_y = static_cast<int>(position.y);

				for (ChannelData channel : channelManagementList_) {
					if (channel.emitterID != entity && soundEffect.soundfile_p != channel.soundfile_p) {
						continue;
					} else {
						alreadyPlaying = true;
						std::cout << "reached already playing \n";
					}
				}

				if (!alreadyPlaying) {
					if (soundEffect.soundfile_p.get() == &footStep_) {
						channelChosen = audioDevice_.emit2D(-1, footStep_, 0); 
						std::cout << "Channel active for step:" << channelChosen << "\n";
						channelManagementList_[channelChosen] = {entity, footStep_ptr_, VOLUME_STANDARD, 0};
					} /*else {
						audioDevice_.emit2D(-1, sniperShot_, 0);
						channelManagementList_[channelChosen] = {entity, sniperShot_ptr_, VOLUME_STANDARD, 0};
					}*/
				}
				ecs.removeComponent<SoundEmitter>(entity);
			}

		}
		for (int i = 0; i < channelManagementList_.size();          // opening for loop like this because I need an index. maybe manual index setting above for loop is
																	// actually easier. No enumerate option in c++ like in python as far as i could
																	// make out
		     i++) { 
			if (!audioDevice_.isChannelPlaying(i)) {
				ChannelData &openedChannelData = channelManagementList_[i];
				openedChannelData.emitterID = -1;
				openedChannelData.soundfile_p = NULL;
				openedChannelData.volume = NULL;
			}
		}
	}

  private:
	struct ChannelData {
		uint32_t emitterID;
		std::shared_ptr<SoundEffect> soundfile_p;
		int volume;
		int assingedGroup;
		// SoundEffect *soundfile_p; // raw pointer usage because of non ownership (chatgpt´s advice), we get a problem
		// with cereal in the Soundemitterstruct though.#
	};

	std::array<ChannelData, VIRTUAL_CHANNELS> channelManagementList_ =
	    {}; // we test channel management here and see if we need to bring some to the engine

	/* static void channelFinished(int channel)
	{
	    instance->channelManagementList_[channel] = ;
	} // callback should work but to have this work properly we need to save more data in our channel management
	  // container, this goes engineside anyhow because we need to wrap mixer.*/

	// static AudioSystem *instance;
	const Engine &engine_;
	const Audio &audioDevice_ =
	    engine_
	        .getAudioDevice(); // let´s try to change this to only need the audio and not the whole engine -> low prio
	Music backgroundMusic_;
	SoundEffect footStep_;
	std::shared_ptr<SoundEffect> footStep_ptr_ = std::make_shared<SoundEffect>(std::move(footStep_));
	SoundEffect sniperShot_;
	std::shared_ptr<SoundEffect> sniperShot_ptr_ = std::make_shared<SoundEffect>(std::move(footStep_));
};