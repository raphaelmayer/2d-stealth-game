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
		// assumes that game starts in main menu -> TODO: wrong assumption, needs to come back when switching back to main menu
	};

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		// Set Background Music according to Gamestate
		const GameState &gameState = gameStateManager_.getGameState();
		switch ()
		if (!audio_.getChannelManager().isStreaming()) {
			backgroundMusic_ = audio_.loadMusicFile(BACKGROUND_JUNGLE_AMBIENCE);
			audio_.streamMusic(backgroundMusic_, -1);
		}

		// testing to check for isMoving here or not could work well
		const std::set<Easys::Entity> &entities = ecs.getEntities();
		for (Easys::Entity entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity)) {
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
		}
		for (const Easys::Entity &entity : entities) {

			if (ecs.hasComponent<SoundEmitter>(entity)) {
				SoundEmitter soundEffect = ecs.getComponent<SoundEmitter>(entity);
				Vec2f &emitterPosition = ecs.getComponent<Positionable>(entity).position;
				Vec2f listenerPosition = camera_.getPosition() + (Utils::toFloat(game_.getScreenSize()) / 2);
				if (soundEffect.soundFile_Ptr == footStep_Ptr_ && entity == PLAYER) {
					audio_.emit3D(entity, footStep_Ptr_, emitterPosition, listenerPosition, {});
				} else if (soundEffect.soundFile_Ptr == akShot_Ptr_) {
					audio_.emit3D(entity, akShot_Ptr_, emitterPosition, listenerPosition, {});
				}
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
};