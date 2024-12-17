#pragma once

#include "ai/MoveToNode.hpp"
#include "components/AI.hpp"
#include "constants.hpp"
#include "ecs/ECSManager.hpp"
#include "ecs/Entity.hpp"
#include "engine/Engine.hpp"
#include "entities/item.hpp"
#include "entities/npc.hpp"
#include "entities/player.hpp"
#include "entities/sign.hpp"
#include "modules/Camera.hpp"
#include "modules/GameStateManager.hpp"
#include "modules/MapManager.hpp"
#include "modules/MenuStack.hpp"
#include "modules/SaveGameManager.hpp"
#include "SDL_mixer.h"
#include "systems/AISystem.hpp"
#include "systems/AudioSystem.hpp"
#include "systems/DebugSystem.hpp"
#include "systems/InputSystem.hpp"
#include "systems/InteractionSystem.hpp"
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
	Game(const std::string title, Vec2d screenSize, Vec2d pixelSize, int frameRate)
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

		saveGameManager.load(WORLD_DEFINITION_PATH);

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

			createTestEntity();

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
			physicsSystem->update(ecs, deltaTime);

			// must happen in between update of physicsystem and rendersystem, or will result in flickering
			camera.update(ecs.getComponent<Positionable>(PLAYER).position);

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
		inputSystem = std::make_unique<InputSystem>(*this);
		aiSystem = std::make_unique<AISystem>(mapManager);
		physicsSystem = std::make_unique<PhysicsSystem>(mapManager);
		interactionSystem = std::make_unique<InteractionSystem>(*this, menuStack);
		renderSystem = std::make_unique<RenderSystem>(*this, mapManager, camera, spritesheet);
		progressSystem = std::make_unique<ProgressSystem>();
		audioSystem = std::make_unique<AudioSystem>(PLAYER);
		debugSystem = std::make_unique<DebugSystem>(*this, mapManager, camera);
	}

	void createTestEntity() { 
		Entity e = instantiateNPCEntity(ecs, {15, 6});
		RigidBody &c = ecs.getComponent<RigidBody>(e);
		//c.endPosition = {16 * TILE_SIZE, 6 * TILE_SIZE};
		//c.isMoving = true;
		
		// This was/is for the custom BT implementation 
		//auto moveToNode = std::make_unique<MoveToNode>();
		//auto btree = std::make_shared<BehaviorTree>(std::move(moveToNode));
		//AI aiComp{Blackboard(ecs, e)};
		//aiComp.targetPosition = {16 * TILE_SIZE, 6 * TILE_SIZE};
		////aiComp.behaviorTree = btree;
		//ecs.addComponent<AI>(e, aiComp);
		
		Vision vision;
		AI ai;
		ecs.addComponent<Vision>(e, vision);
		ecs.addComponent<AI>(e, ai);
	}

	SDL_Texture *spritesheet;
	std::shared_ptr<SDL_Texture> mainMenuBackground;
	ECSManager ecs;
	MapManager mapManager;
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
};