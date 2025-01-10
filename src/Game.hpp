#pragma once

#include "SDL_mixer.h"
#include "components/AI.hpp"
#include "components/Patrol.hpp"
#include "constants.hpp"
#include "ecs/ECSManager.hpp"
#include "ecs/Entity.hpp"
#include "engine/Engine.hpp"
#include "entities/item.hpp"
#include "entities/npc.hpp"
#include "entities/player.hpp"
#include "entities/sign.hpp"
#include "modules/BTManager.hpp"
#include "modules/Camera.hpp"
#include "modules/GameStateManager.hpp"
#include "modules/MapManager.hpp"
#include "modules/MenuStack.hpp"
#include "modules/SaveGameManager.hpp"
#include "systems/AISystem.hpp"
#include "systems/AudioSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/InputSystem.hpp"
#include "systems/InteractionSystem.hpp"
#include "systems/PathfindingSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/ProgressSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/InGameMenu.hpp"
#include "ui/MainMenu.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <thread>

class Game : public Engine {
  public:
	Game(const std::string title, Vec2i screenSize, Vec2i pixelSize, int frameRate)
	    : Engine(title, screenSize, pixelSize, frameRate), camera(screenSize.x, screenSize.y)
	{
	}

	bool onStart() override
	{
		spritesheet = loadTexture(SPRITE_SHEET);
		if (!spritesheet) {
			throw std::runtime_error("Error. InputSystemUnable to open overworld spritesheet");
		}
		mainMenuBackground = std::shared_ptr<SDL_Texture>(loadTexture(MAINMENU_BACKGROUND), SDL_DestroyTexture);

		mapManager.loadMap(0);

		// saveGameManager.load(WORLD_DEFINITION_PATH);

		initializeSystems();

		return true;
	}

	bool onUpdate(double deltaTime) override
	{
		switch (gameStateManager.getGameState()) {

		case GameState::MAINMENU: {
			drawTexture(mainMenuBackground);

			if (menuStack.isEmpty())
				menuStack.push(std::make_unique<MainMenu>(*this, gameStateManager, saveGameManager, menuStack));

			menuStack.update();

			createTestEntity({15, 6}, {{Vec2i{15, 6} * TILE_SIZE, Rotation::SOUTH, 2},
			                           {Vec2i{19, 5} * TILE_SIZE, Rotation::NORTH, 2},
			                           {Vec2i{24, 7} * TILE_SIZE, Rotation::EAST, 2}});
			createTestEntity({21, 7}, {{Vec2i{21, 7} * TILE_SIZE, Rotation::SOUTH, 2},
			                           {Vec2i{21, 8} * TILE_SIZE, Rotation::NORTH, 2}});
			createTestEntity({14, 7}, {{Vec2i{14, 7} * TILE_SIZE, Rotation::EAST, 5},
			                           {Vec2i{20, 7} * TILE_SIZE, Rotation::WEST, 5}});
			createTestEntity({16, 6}, {{Vec2i{16, 6} * TILE_SIZE, Rotation::SOUTH, 0},
			                           {Vec2i{19, 6} * TILE_SIZE, Rotation::NORTH, 0},
			                           {Vec2i{19, 7} * TILE_SIZE, Rotation::EAST, 0},
			                           {Vec2i{16, 7} * TILE_SIZE, Rotation::WEST, 0}});

			break;
		}

		case GameState::PLAYING: {
			if (getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed && ecs.hasComponent<Controllable>(0)) {
				if (!menuStack.isEmpty())
					menuStack.reset();
				menuStack.push(std::make_unique<InGameMenu>(*this, ecs, gameStateManager, saveGameManager, menuStack));
				ecs.removeComponent<Controllable>(0);
			}

			inputSystem->update(ecs, deltaTime);
			aiSystem->update(ecs, deltaTime);
			pathfindingSystem->update(ecs, deltaTime);
			physicsSystem->update(ecs, deltaTime);

			// camera.focus(ecs.getComponent<Positionable>(PLAYER).position);

			// must happen in between update of physicsystem and rendersystem, or will result in flickering
			renderSystem->update(ecs, deltaTime);

			// Needs to happen after rendering entities to be on top but before interactionsystem,
			// otherwise same input might close just opened dialogue.
			menuStack.update();

			interactionSystem->update(ecs, deltaTime);
			audioSystem->update(ecs, deltaTime);
			progressSystem->update(ecs, deltaTime);
			debugSystem->update(ecs, deltaTime);

			break;
		}
		}

		return true;
	}

	bool onDestroy() override
	{
		// deinitialize anything game-related here
		return true;
	}

  private:
	void initializeSystems()
	{
		inputSystem = std::make_unique<InputSystem>(*this, camera);
		aiSystem = std::make_unique<AISystem>(btManager, mapManager);
		physicsSystem = std::make_unique<PhysicsSystem>(mapManager);
		interactionSystem = std::make_unique<InteractionSystem>(*this, menuStack);
		renderSystem = std::make_unique<RenderSystem>(*this, mapManager, camera, spritesheet);
		progressSystem = std::make_unique<ProgressSystem>();
		audioSystem = std::make_unique<AudioSystem>(PLAYER);
		debugSystem = std::make_unique<DebugSystem>(*this, mapManager, camera);
		pathfindingSystem = std::make_unique<PathfindingSystem>(mapManager);
	}

	void createTestEntity(const Vec2i &position, const std::vector<PatrolPoint> &waypoints)
	{
		Entity e = instantiateNPCEntity(ecs, position);
		ecs.addComponent<Vision>(e, Vision{});
		ecs.addComponent<AI>(e, AI{});
		ecs.addComponent<Patrol>(e, Patrol{waypoints});
		btManager.createTreeForEntity(e, "MainTree");
	}

	SDL_Texture *spritesheet;
	std::shared_ptr<SDL_Texture> mainMenuBackground;
	ECSManager ecs;
	MapManager mapManager;
	BTManager btManager = BTManager(ecs);
	SaveGameManager saveGameManager = SaveGameManager(ecs);
	GameStateManager gameStateManager;
	MenuStack menuStack;
	Camera camera;

	std::unique_ptr<InputSystem> inputSystem;
	std::unique_ptr<AISystem> aiSystem;
	std::unique_ptr<PhysicsSystem> physicsSystem;
	std::unique_ptr<InteractionSystem> interactionSystem;
	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<ProgressSystem> progressSystem;
	std::unique_ptr<AudioSystem> audioSystem;
	std::unique_ptr<DebugSystem> debugSystem;
	std::unique_ptr<PathfindingSystem> pathfindingSystem;
};