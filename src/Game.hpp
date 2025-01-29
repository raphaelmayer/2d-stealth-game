#pragma once

#include "SDL_mixer.h"
#include "components/AI.hpp"
#include "components/Patrol.hpp"
#include "components/Projectile.hpp"
#include "constants.hpp"
#include "ecs/ECSManager.hpp"
#include "ecs/Entity.hpp"
#include "engine/Engine.hpp"
#include "entities/item.hpp"
#include "entities/npc.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/sign.hpp"
#include "map/MapManagerTMX.hpp"
#include "modules/BTManager.hpp"
#include "modules/Camera.hpp"
#include "modules/GameStateManager.hpp"
#include "modules/SaveGameManager.hpp"
#include "systems/AISystem.hpp"
#include "systems/AudioSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/InputSystem.hpp"
#include "systems/InteractionSystem.hpp"
#include "systems/PathfindingSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/ProgressSystem.hpp"
#include "systems/ProjectileSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "ui/InGameMenu.hpp"
#include "ui/MainMenu.hpp"
#include "ui/MenuStack.hpp"
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
		mapManager.loadMap(0);
		initializeSystems();
		return true;
	}

	bool onUpdate(double deltaTime) override
	{
		switch (gameStateManager.getGameState()) {

		case GameState::MAINMENU: {
			if (menuStack.isEmpty())
				menuStack.push(std::make_unique<MainMenu>(*this, gameStateManager, saveGameManager, menuStack));

			menuStack.update();

			break;
		}

		case GameState::PLAYING: {
			if (getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed && ecs.hasComponent<Controllable>(0)) {
				if (!menuStack.isEmpty())
					menuStack.reset();
				menuStack.push(std::make_unique<InGameMenu>(*this, ecs, gameStateManager, saveGameManager, menuStack));
				ecs.removeComponent<Controllable>(0);
			}

			if (!addedEntities) {
				instantiatePlayerEntity(ecs, {17, 17});
				addTestEntities();
				addedEntities = true;
			}

			inputSystem->update(ecs, deltaTime);
			aiSystem->update(ecs, deltaTime);
			pathfindingSystem->update(ecs, deltaTime);
			physicsSystem->update(ecs, deltaTime);

			//camera.focus(ecs.getComponent<Positionable>(PLAYER).position);

			// must happen in between update of physicsystem and rendersystem, or will result in flickering
			renderSystem->update(ecs, deltaTime);

			// Needs to happen after rendering entities to be on top but before interactionsystem,
			// otherwise same input might close just opened dialogue.
			menuStack.update();

			interactionSystem->update(ecs, deltaTime);
			//audioSystem->update(ecs, deltaTime);
			//progressSystem->update(ecs, deltaTime);
			debugSystem->update(ecs, deltaTime);
			projectileSystem->update(ecs, deltaTime);

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
		renderSystem = std::make_unique<RenderSystem>(*this, mapManager, camera);
		progressSystem = std::make_unique<ProgressSystem>();
		audioSystem = std::make_unique<AudioSystem>(PLAYER);
		debugSystem = std::make_unique<DebugSystem>(*this, mapManager, camera);
		pathfindingSystem = std::make_unique<PathfindingSystem>(mapManager);
		projectileSystem = std::make_unique<ProjectileSystem>();
	}

	void createTestEntity(const Vec2i &position, const std::vector<PatrolPoint> &waypoints)
	{
		Entity e = instantiateNPCEntity(ecs, position);
		ecs.addComponent<Vision>(e, Vision{});
		ecs.addComponent<AI>(e, AI{position});
		ecs.addComponent<Patrol>(e, Patrol{waypoints});
		btManager.createTreeForEntity(e, "MainTree");
	}

	void addTestEntities()
	{
		createTestEntity(
		    {15, 6}, {{Vec2i{10, 3} * TILE_SIZE, Rotation::SOUTH, 2}, {Vec2i{2, 3} * TILE_SIZE, Rotation::SOUTH, 6}});
		createTestEntity({2, 2}, {{Vec2i{9, 3} * TILE_SIZE, Rotation::SOUTH, 2},
		                           {Vec2i{6, 2} * TILE_SIZE, Rotation::SOUTH, 6},
		                           {Vec2i{5, 3} * TILE_SIZE, Rotation::SOUTH, 6}});
		createTestEntity(
		    {16, 7}, {{Vec2i{11, 5} * TILE_SIZE, Rotation::SOUTH, 5}, {Vec2i{11, 14} * TILE_SIZE, Rotation::NORTH, 5}});
		createTestEntity(
		    {14, 7}, {{Vec2i{13, 11} * TILE_SIZE, Rotation::SOUTH, 60}});
		createTestEntity({16, 6}, {{Vec2i{0, 6} * TILE_SIZE, Rotation::SOUTH, 0},
		                           {Vec2i{9, 6} * TILE_SIZE, Rotation::NORTH, 0},
		                           {Vec2i{9, 13} * TILE_SIZE, Rotation::EAST, 0},
		                           {Vec2i{0, 13} * TILE_SIZE, Rotation::WEST, 0}});
	}

	ECSManager ecs;
	MapManager mapManager;
	BTManager btManager = BTManager(ecs);
	SaveGameManager saveGameManager = SaveGameManager(ecs);
	GameStateManager gameStateManager;
	MenuStack menuStack;
	Camera camera;
	bool addedEntities = false;

	std::unique_ptr<InputSystem> inputSystem;
	std::unique_ptr<AISystem> aiSystem;
	std::unique_ptr<PhysicsSystem> physicsSystem;
	std::unique_ptr<InteractionSystem> interactionSystem;
	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<ProgressSystem> progressSystem;
	std::unique_ptr<AudioSystem> audioSystem;
	std::unique_ptr<DebugSystem> debugSystem;
	std::unique_ptr<PathfindingSystem> pathfindingSystem;
	std::unique_ptr<ProjectileSystem> projectileSystem;
};