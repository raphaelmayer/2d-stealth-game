#include "../components/SoundEmitter.hpp"
#include "../modules/GameStateManager.hpp"
#include "System.hpp"
#include <SDL.h>
#include <easys/easys.hpp>
#include <memory>
#include <stdint.h>


//TODO: comment on this system being special
class AudioSystem final : public System {
  public:
	explicit AudioSystem(Engine &game, const GameStateManager &gameStateManager, const Camera &camera)
	    : game_(game), gameStateManager_(gameStateManager), camera_(camera)
	{
		audio_.setVolume(50);
	};

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		// Set Background Music according to Gamestate
		const GameState &gameState = gameStateManager_.getGameState();
		switch (gameState) {
			case GameState::MAINMENU:
				
				if (!audio_.getChannelManager().isThisStreaming(mainMenuMusic_Ptr_)) {
					audio_.streamMusic(mainMenuMusic_Ptr_, -1);
				}
			    break;
			case GameState::PLAYING:
				if (audio_.getChannelManager().whatIsStreaming() != backgroundMusic_Ptr_) {
					audio_.streamMusic(backgroundMusic_Ptr_, -1);
				}
			    break;
		}

		// testing to check for isMoving here or not could work well
		const std::vector<Easys::Entity> &entitiesRigidBody = ecs.getEntitiesByComponent<RigidBody>();
		for (Easys::Entity entity : entitiesRigidBody) {
			RigidBody &rigidBody = ecs.getComponent<RigidBody>(entity);
			if (rigidBody.isMoving) {
				if (!ecs.hasComponent<SoundEmitter>(entity)) {
					ecs.addComponent<SoundEmitter>(entity, {footStep_Ptr_}); // TODO --> MOVE TO RELEVANT SYSTEM
				}
			}
			if (rigidBody.isShooting) {
				if (!ecs.hasComponent<SoundEmitter>(entity)) {
					ecs.addComponent<SoundEmitter>(entity, {akShot_Ptr_}); // TODO --> MOVE TO RELEVANT SYSTEM
					rigidBody.isShooting = false;                          // move to input system or whereever
				}
			}	
		}
		const std::vector<Easys::Entity> &entitiesEquippedWeapon = ecs.getEntitiesByComponent<EquippedWeapon>();
		for (Easys::Entity entity : entitiesEquippedWeapon) {
			EquippedWeapon &equippedWeapon = ecs.getComponent<EquippedWeapon>(entity);
			if (equippedWeapon.isReloading) {
				ecs.addComponent<SoundEmitter>(entity, {akReload_Ptr_});
				equippedWeapon.isReloading = false; //bandaid fix
			}
		}

		const std::vector<Easys::Entity> &entitiesSoundEmitter = ecs.getEntitiesByComponent<SoundEmitter>();
		for (const Easys::Entity &entity : entitiesSoundEmitter) {

			SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
			Vec2f &emitterPosition = ecs.getComponent<Positionable>(entity).position;
			Vec2f listenerPosition = camera_.getPosition() + (Utils::toFloat(game_.getScreenSize()) / 2);
			if (soundEffect.soundFile_Ptr == footStep_Ptr_ && entity == PLAYER) {
				audio_.emit3D(entity, footStep_Ptr_, emitterPosition, listenerPosition, {});
			} else {
				audio_.emit3D(entity, soundEffect.soundFile_Ptr, emitterPosition, listenerPosition, {});
				
			}
			ecs.removeComponent<SoundEmitter>(entity);
		}
	}

  private:
	Engine &game_;
	const GameStateManager &gameStateManager_;
	Audio &audio_ =
	    game_
	        .getAudio(); // let´s try to change this to only need the audio and not the whole engine -> low prio
	const Camera &camera_;

	// internal types and pointers
	std::shared_ptr<Music> mainMenuMusic_Ptr_ =
	    std::make_shared<Music>(audio_.loadMusicFile(BACKGROUND_MAIN_MENU));
	std::shared_ptr<Music> backgroundMusic_Ptr_ = std::make_shared<Music>(audio_.loadMusicFile(BACKGROUND_JUNGLE_AMBIENCE));
	std::shared_ptr<SoundEffect> footStep_Ptr_ = std::make_shared<SoundEffect>(
	    audio_.loadSoundEffectFile(SFX_FOOTSTEP)); // probably SoundEffect should be a pointer by itself?
	std::shared_ptr<SoundEffect> akShot_Ptr_ =
	    std::make_shared<SoundEffect>(audio_.loadSoundEffectFile(SFX_AK_SHOT_SINGLE));
	std::shared_ptr<SoundEffect> akReload_Ptr_ =
	    std::make_shared<SoundEffect>(audio_.loadSoundEffectFile(SFX_AK_RELOAD));
};