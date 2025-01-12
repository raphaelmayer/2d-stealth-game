#include "../components/Audio.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"
#include <SDL.h>

#define BACKGROUND_MUSIC_FADE_MS 700

class AudioSystem final : public System {
  public:
	void update(ECSManager &ecs, const double deltaTime) override
	{
		// Play SFX
		const std::set<Entity> &entities = ecs.getEntities();
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Audio>(entity)) {
				playSFX(ecs.getComponent<Audio>(entity).file.c_str());
				ecs.removeComponent<Audio>(entity);
			}
		}
		// Change background music depending on the players position
		Vec2i playerPosition = ecs.getComponent<Positionable>(player_).position;
		switch (currentLevel) {
		case 1:
			if (playerPosition.x > LEVEL_2_WEST_BORDER * TILE_SIZE) {
				playBackgroundMusic(BACKGROUND_MUSIC_2);
				currentLevel = 2;
			}
			break;
		case 2:
			if (playerPosition.x < LEVEL_1_EAST_BORDER * TILE_SIZE) {
				playBackgroundMusic(BACKGROUND_MUSIC_1);
				currentLevel = 1;
			}
			break;
		}
	}
	explicit AudioSystem(const Entity player)
	{
		player_ = player;
		currentLevel = 1;
		playBackgroundMusic(BACKGROUND_MUSIC_1);
	}

  private:
	// use next free channel to play sfx
	static void playSFX(const char *file)
	{
		auto chunk = Mix_LoadWAV(file);
		if (chunk) {
			Mix_PlayChannel(-1, chunk, 0);
		} else {
			fprintf(stderr, "Failed to load sfx sound at: %s\n%s", file, Mix_GetError());
		}
	}

	void playBackgroundMusic(const char *file)
	{
		// Although documented differently, this method does block and therefore stop other systems from updating
		// causing to stop the player from walking
		// Mix_FadeOutMusic(BACKGROUND_MUSIC_FADE_MS);
		backgroundMusic = Mix_LoadMUS(file);
		if (backgroundMusic) {
			Mix_FadeInMusic(backgroundMusic, -1, BACKGROUND_MUSIC_FADE_MS);
		} else {
			fprintf(stderr, "Failed to load background music at: %s\n%s", file, Mix_GetError());
		}
	}

	Entity player_;
	uint16_t currentLevel;
	Mix_Music *backgroundMusic;
};