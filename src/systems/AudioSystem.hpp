#include "../components/SoundEmitter.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"
#include <SDL.h>
#include <memory>
#include <stdint.h>

class AudioSystem final : public System {
  public:
	explicit AudioSystem(const Engine &engine, const Camera &camera) : engine_(engine), camera_(camera)
	{
		footStep_ = audioDevice_.loadSoundEffectFile(SFX_FOOTSTEP);
		sniperShot_ = audioDevice_.loadSoundEffectFile(SFX_SNIPER_SHOT_AND_RELOAD);

		audioDevice_.setVolume(-1, DEFAULT_VOLUME);
	};

	void update(ECSManager &ecs, const double deltaTime) override
	{
		// Start Background Music at Start of Game loop
		if (!backgroundMusic_) {
			backgroundMusic_ = audioDevice_.loadMusicFile(BACKGROUND_MUSIC_1);
			// audioDevice_.streamMusic(backgroundMusic_, -1);
		}

		// testing to check for isMoving here or not could work well
		const std::set<Entity> &entities = ecs.getEntities();
		for (Entity entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity)) {
				RigidBody &rigidBody = ecs.getComponent<RigidBody>(entity);
				if (rigidBody.isMoving) {  
					if (!ecs.hasComponent<SoundEmitter>(entity)) {
						ecs.addComponent<SoundEmitter>(entity, {footStep_Ptr_}); //TODO --> MOVE TO RELEVANT SYSTEM
					}
				}
				if (rigidBody.isShooting) {
					if (!ecs.hasComponent<SoundEmitter>(entity)) {
						ecs.addComponent<SoundEmitter>(entity, {sniperShot_Ptr_}); //TODO --> MOVE TO RELEVANT SYSTEM
						rigidBody.isShooting = false; // move to input system or whereever
					}
				}
			}
		}
		int counter = 0;
		int channelChosen;
		bool alreadyPlaying = false;
		for (const Entity &entity : entities) {

			if (ecs.hasComponent<SoundEmitter>(entity)) {
				SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
				Vec2f &emitterPosition = ecs.getComponent<Positionable>(entity).position;
				Vec2f listenerPosition = camera_.getPosition() + (Utils::toFloat(engine_.getScreenSize()) / 2);

				for (ChannelData channel : channelManagementList_) {
					if (channel.emitterID == entity && soundEffect.soundFile_Ptr == channel.activeTrack_Ptr) {
						alreadyPlaying = true;
						break;
					}
				}

				if (!alreadyPlaying && entity != PLAYER) { // remove 2nd check here, just exists for testing 3d!
					if (soundEffect.soundFile_Ptr == footStep_Ptr_) {
						channelChosen = audioDevice_.emit2D(-1, footStep_, 0);
						channelManagementList_[channelChosen] = {entity, footStep_Ptr_, DEFAULT_VOLUME, 0};
					} else if (soundEffect.soundFile_Ptr == sniperShot_Ptr_) {
						channelChosen = audioDevice_.emit2D(-1, sniperShot_, 0);
						channelManagementList_[channelChosen] = {entity, sniperShot_Ptr_, DEFAULT_VOLUME, 0};
					}
				}
				/*if (entity == PLAYER) {
					audioDevice_.emit3D(footStep_, emitterPosition, listenerPosition, 0);
				}*/
				ecs.removeComponent<SoundEmitter>(entity);
			}
		}
		for (int i = 0; i < channelManagementList_.size(); i++) {
			if (!audioDevice_.isChannelPlaying(i)) {
				ChannelData &openedChannelData = channelManagementList_[i];
				openedChannelData.emitterID = -1;
				openedChannelData.activeTrack_Ptr = nullptr;
				openedChannelData.volume = 0;
			}
		}
	}

  private:
	// we test channel management here and see if we need to bring some to the engine
	struct ChannelData {
		uint32_t emitterID;
		std::shared_ptr<SoundEffect> activeTrack_Ptr; // point of contention
		int volume;
		int assingedGroup; //stays as int
	};

	std::array<ChannelData, VIRTUAL_CHANNELS> channelManagementList_ = {};

	const Engine &engine_;
	const Audio &audioDevice_ =
	    engine_
	        .getAudioDevice(); // let´s try to change this to only need the audio and not the whole engine -> low prio
	const Camera &camera_;

	// internal types and pointers
	Music backgroundMusic_;
	SoundEffect footStep_;
	std::shared_ptr<SoundEffect> footStep_Ptr_ = std::make_shared<SoundEffect>(std::move(footStep_));
	SoundEffect sniperShot_;
	std::shared_ptr<SoundEffect> sniperShot_Ptr_ = std::make_shared<SoundEffect>(std::move(footStep_));
};