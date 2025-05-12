#pragma once

#include "SDL_mixer.h"
#include "components/Patrol.hpp"
#include "components/Projectile.hpp"
#include "constants.hpp"
#include "engine/Engine.hpp"
#include "entities/item.hpp"
#include "entities/npc.hpp"
#include "entities/player.hpp"
#include "entities/projectile.hpp"
#include "entities/sign.hpp"
#include "map/MapManager.hpp"
#include "modules/BTManager.hpp"
#include "modules/Camera.hpp"
#include "modules/GameStateManager.hpp"
#include "modules/SaveGameManager.hpp"
#include "modules/SelectionManager.hpp"
#include "systems/AISystem.hpp"
#include "systems/AnimationSystem.hpp"
#include "systems/AudioSystem.hpp"
#include "systems/CleanupSystem.hpp"
#include "systems/DamageSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/FiringSystem.hpp"
#include "systems/InputSystem.hpp"
#include "systems/PathfindingSystem.hpp"
#include "systems/PhysicsSystem.hpp"
#include "systems/ProjectileSystem.hpp"
#include "systems/RenderSystem.hpp"
#include "systems/UIRenderSystem.hpp"
#include "ui/InGameMenu.hpp"
#include "ui/MainMenu.hpp"
#include "ui/MenuStack.hpp"
#include "ui/SelectionUIElement.hpp"
#include <chrono>
#include <easys/easys.hpp>
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
			audioSystem->update(ecs, deltaTime);
			break;
		}

		case GameState::PLAYING: {
			// move this somewhere? maybe InputSystem? Passing everything down to the UI elements would mean input
			// system needsthose references aswell. this does not seem to be a good solution.
			if (getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed) {
				if (!menuStack.isEmpty())
					menuStack.reset();
				else
					menuStack.push(
					    std::make_unique<InGameMenu>(*this, ecs, gameStateManager, saveGameManager, menuStack));
			}

			if (!addedEntities) {
				addTestEntities();
				addedEntities = true;
				camera.focus(ecs.getComponent<Positionable>(PLAYER).position);
			}

			inputSystem->update(ecs, deltaTime);
			aiSystem->update(ecs, deltaTime);
			pathfindingSystem->update(ecs, deltaTime);
			firingSystem->update(ecs, deltaTime);
			physicsSystem->update(ecs, deltaTime);
			damageSystem->update(ecs, deltaTime);

			// camera.focus(ecs.getComponent<Positionable>(PLAYER).position);

			// must happen in between update of physicsystem and rendersystem, or will result in flickering
			animationSystem->update(ecs, deltaTime);
			renderSystem->update(ecs, deltaTime);

			// Needs to happen after rendering entities to be rendered on top
			uiRenderSystem->update(ecs, deltaTime);

			audioSystem->update(ecs, deltaTime);
			// progressSystem->update(ecs, deltaTime);
			debugSystem->update(ecs, deltaTime);
			projectileSystem->update(ecs, deltaTime);

			cleanupSystem->update(ecs, deltaTime);

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
		inputSystem = std::make_unique<InputSystem>(*this, camera, selectionManager);
		aiSystem = std::make_unique<AISystem>(btManager, mapManager);
		physicsSystem = std::make_unique<PhysicsSystem>(mapManager);
		renderSystem = std::make_unique<RenderSystem>(*this, mapManager, camera, selectionManager);
		uiRenderSystem = std::make_unique<UIRenderSystem>(*this, menuStack, selectionManager);
		audioSystem = std::make_unique<AudioSystem>(*this, gameStateManager, camera);
		debugSystem = std::make_unique<DebugSystem>(*this, mapManager, camera);
		pathfindingSystem = std::make_unique<PathfindingSystem>(mapManager);
		projectileSystem = std::make_unique<ProjectileSystem>(mapManager);
		firingSystem = std::make_unique<FiringSystem>(*this);
		animationSystem = std::make_unique<AnimationSystem>(*this, mapManager, camera);
		damageSystem = std::make_unique<DamageSystem>();
		cleanupSystem = std::make_unique<CleanupSystem>();
	}

	void createTestEntity(const Vec2i &position, const std::vector<PatrolPoint> &waypoints)
	{
		Easys::Entity e = instantiateNPCEntity(ecs, position);
		ecs.addComponent<Patrol>(e, Patrol{waypoints});
		btManager.createTreeForEntity(e, "MainTree");
	}

	void addTestEntities()
	{
		instantiatePlayerEntity(ecs, {2, 55});
		instantiatePlayerEntity(ecs, {3, 55});
		instantiatePlayerEntity(ecs, {2, 56});
		instantiatePlayerEntity(ecs, {3, 56});

		createTestEntity({12, 28}, {{Vec2i{17, 32} * TILE_SIZE, Rotation::SOUTH, 6},
		                            {Vec2i{12, 28} * TILE_SIZE, Rotation::SOUTH, 6}});
		createTestEntity({20, 28}, {{Vec2i{30, 23} * TILE_SIZE, Rotation::EAST, 2},
		                            {Vec2i{27, 28} * TILE_SIZE, Rotation::SOUTH, 6},
		                            {Vec2i{20, 28} * TILE_SIZE, Rotation::SOUTH, 6}});
		createTestEntity({24, 20}, {{Vec2i{27, 15} * TILE_SIZE, Rotation::EAST, 5},
		                            {Vec2i{19, 15} * TILE_SIZE, Rotation::NORTH, 5}});
		createTestEntity({21, 37}, {{Vec2i{21, 37} * TILE_SIZE, Rotation::SOUTH, 60}});
		createTestEntity({12, 16}, {{Vec2i{6, 23} * TILE_SIZE, Rotation::SOUTH, 0},
		                            {Vec2i{9, 31} * TILE_SIZE, Rotation::SOUTH, 0},
		                            {Vec2i{19, 31} * TILE_SIZE, Rotation::SOUTH, 0},
		                            {Vec2i{20, 24} * TILE_SIZE, Rotation::SOUTH, 0}});
	}

	Easys::ECS ecs;
	MapManager mapManager;
	BTManager btManager = BTManager(ecs);
	SaveGameManager saveGameManager = SaveGameManager(ecs);
	SelectionManager selectionManager;
	GameStateManager gameStateManager;
	MenuStack menuStack; // should this even live here? For now we keep it here since we might want other systems to
	                     // push menus aswell.
	Camera camera;
	bool addedEntities = false;

	std::unique_ptr<InputSystem> inputSystem;
	std::unique_ptr<AISystem> aiSystem;
	std::unique_ptr<PhysicsSystem> physicsSystem;
	std::unique_ptr<RenderSystem> renderSystem;
	std::unique_ptr<UIRenderSystem> uiRenderSystem;
	std::unique_ptr<AudioSystem> audioSystem;
	std::unique_ptr<DebugSystem> debugSystem;
	std::unique_ptr<PathfindingSystem> pathfindingSystem;
	std::unique_ptr<ProjectileSystem> projectileSystem;
	std::unique_ptr<FiringSystem> firingSystem;
	std::unique_ptr<AnimationSystem> animationSystem;
	std::unique_ptr<DamageSystem> damageSystem;
	std::unique_ptr<CleanupSystem> cleanupSystem;
};