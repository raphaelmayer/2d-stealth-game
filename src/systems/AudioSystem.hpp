#include "../components/SoundEmitter.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"
#include <SDL.h>
#include <memory>
#include <stdint.h>

class AudioSystem final : public System {
  public:
	explicit AudioSystem(Engine &engine, const Camera &camera) : engine_(engine), camera_(camera)
	{
		audioDevice_.setVolume(50);
		//assumes that game starts in main menu
		audioDevice_.streamMusic(mainMenuMusic_, -1);
	};

	void update(ECSManager &ecs, const double deltaTime) override
	{
		// Start Ingame Background Music at Start of Game loop 
		if (!backgroundMusic_) {
			backgroundMusic_ = audioDevice_.loadMusicFile(BACKGROUND_JUNGLE_AMBIENCE);
			audioDevice_.streamMusic(backgroundMusic_, -1);
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
						ecs.addComponent<SoundEmitter>(entity, {akShot_Ptr_}); //TODO --> MOVE TO RELEVANT SYSTEM
						rigidBody.isShooting = false; // move to input system or whereever
					}
				}
				//this part stops emission of shot sounds when reloading -> Hack, TODO --> enable loading and randomizing
				if (ecs.hasComponent<EquippedWeapon>(entity)) {
					if (ecs.getComponent<EquippedWeapon>(entity).isReloading) {
						int channelToHalt = audioDevice_.getChannelManager().whereIsEmitterPlayingThis(entity, akShot_Ptr_);
						audioDevice_.stopEmission(channelToHalt);
					}

				}
			}
		}
		for (const Entity &entity : entities) {

			if (ecs.hasComponent<SoundEmitter>(entity)) {
				SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
				Vec2f &emitterPosition = ecs.getComponent<Positionable>(entity).position;
				Vec2f listenerPosition = camera_.getPosition() + (Utils::toFloat(engine_.getScreenSize()) / 2);
				if (soundEffect.soundFile_Ptr == footStep_Ptr_ && entity == PLAYER) 
				{
					audioDevice_.emit3D(entity, footStep_Ptr_, emitterPosition, listenerPosition, {});
				} else if (soundEffect.soundFile_Ptr == akShot_Ptr_) 
				{
					audioDevice_.emit3D(entity, akShot_Ptr_, emitterPosition, listenerPosition, {});
				}
			}
			ecs.removeComponent<SoundEmitter>(entity);
			
		}
	}

  private:

	Engine &engine_;
	Audio &audioDevice_ =
	    engine_
	        .getAudioDevice(); // let´s try to change this to only need the audio and not the whole engine -> low prio
	const Camera &camera_;

	// internal types and pointers
	Music mainMenuMusic_ = audioDevice_.loadMusicFile(BACKGROUND_MAIN_MENU);
	Music backgroundMusic_;
	std::shared_ptr<SoundEffect> footStep_Ptr_ =
	    std::make_shared<SoundEffect>(audioDevice_.loadSoundEffectFile(SFX_FOOTSTEP));	//probably SoundEffect should be a pointer by itself?
	std::shared_ptr<SoundEffect> akShot_Ptr_ =
	    std::make_shared<SoundEffect>(audioDevice_.loadSoundEffectFile(SFX_AK_SHOT_FULL_AUTO_LONG));
};