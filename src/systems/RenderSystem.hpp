#include "../components/AI.hpp"
#include "../components/Animatable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../modules/Camera.hpp"
#include "../modules/MapManager.hpp"
#include "System.hpp"
#include <functional>
#include <iostream>

// The RenderSystem is responsible for rendering the map and all entities with Renderable components.
// It performs visibility culling using the camera's position to avoid unnecessary rendering.
class RenderSystem final : public System {
  public:
	RenderSystem(const Engine &engine, const MapManager &mapManager, const Camera &camera, SDL_Texture *spritesheet) 
		: engine_(engine), mapManager_(mapManager), camera_(camera), spritesheet_(spritesheet) 
	{
		weaponTexture = engine_.loadTexture(M4A1);
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		Vec2d camPos = camera_.getPosition(); // Cache the camera's position to ensure consistent camera positioning.
		timeElapsed_ += deltaTime;

		drawMap(camPos);

		const std::set<Entity> &entities = ecs.getEntities();
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Renderable>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity).position;
				auto &renderable = ecs.getComponent<Renderable>(entity);

				Vec2d spriteSrc = renderable.spriteSrc;
				Vec2d size = renderable.size;
				int offset_y = renderable.offset_y;

				if (ecs.hasComponent<Rotatable>(entity)) {
					const auto rotation = ecs.getComponent<Rotatable>(entity).rotation;
					// due to how the spritesheet and the Rotation enum are laid out
					// this corresponds to the sprite for the current direction
					spriteSrc.x = rotation * 2 * size.x;
				}

				if (ecs.hasComponent<Animatable>(entity)) {
					auto &animatable = ecs.getComponent<Animatable>(entity);
					handleAnimation(ecs, entity, animatable, spriteSrc.y);
				}

				SDL_Rect src = {spriteSrc.x, spriteSrc.y, size.x, size.y};
				SDL_Rect dst = {position.x, position.y + offset_y, size.x, size.y};
				// Perform visibility culling before rendering the entity.
				if (isVisibleOnScreen(dst, camPos, engine_.getScreenSize())) {
					SDL_Rect camAdjustedDst = {dst.x - camPos.x, dst.y - camPos.y, dst.w, dst.h};
					engine_.drawSpriteFromSheet(src, camAdjustedDst, spritesheet_);
				
					// Currently testing: rendering weapons
					// Render the weapon if applicable
					// renderWeapon(ecs, entity, position, camPos);
					if (ecs.hasComponent<AI>(entity)) {
						renderAlertnessLevel(position, ecs.getComponent<AI>(entity), camPos);
					}
				}
			}
		}
	}

  private:
	void handleAnimation(ECSManager &ecs, const Entity entity, Animatable &animatable, int &spriteSrcY)
	{
		if (timeElapsed_ > ANIMATION_UPDATE_RATE_IN_SECONDS){
			timeElapsed_ = 0;
			if (ecs.hasComponent<RigidBody>(entity) && ecs.getComponent<RigidBody>(entity).isMoving) {
				animatable.currentAnimation += 1;
				if (animatable.currentAnimation >= PLAYER_NUMBER_ANIMATIONS) {
					animatable.currentAnimation = 0;
				}
			} else {
				animatable.currentAnimation = PLAYER_STANDING_ANIMATION_NUMBER;
			}
		}
		spriteSrcY = animatable.animationAdresses[animatable.currentAnimation];
	}

	bool isVisibleOnScreen(SDL_Rect dst, Vec2d cameraPosition, Vec2d screenSize)
	{
		return dst.x >= (cameraPosition.x - TILE_SIZE) 
			&& dst.x < (cameraPosition.x + screenSize.x)
		    && dst.y >= (cameraPosition.y - TILE_SIZE) 
			&& dst.y < (cameraPosition.y + screenSize.y);
	}

	void drawMap(const Vec2d &camPos)
	{
		const LevelMap &map = mapManager_.getLevelMap();

		// Calculate the range of visible tiles to render based on the camera's position.
		int startX = std::max(0, static_cast<int>(camPos.x / TILE_SIZE));
		int startY = std::max(0, static_cast<int>(camPos.y / TILE_SIZE));
		int endX = std::min(map.getWidth(), static_cast<int>((camPos.x + engine_.getScreenSize().x) / TILE_SIZE) + 1);
		int endY = std::min(map.getHeight(), static_cast<int>((camPos.y + engine_.getScreenSize().y) / TILE_SIZE) + 1);

		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				const std::vector<TileMetadata> fullTiledata = mapManager_.getTileData(x, y);
				for (const TileMetadata &tiledata : fullTiledata) {
					Vec2d srcPos = {tiledata.spriteSheetX, tiledata.spriteSheetY};
					SDL_Rect src = {srcPos.x * TILE_SIZE, srcPos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
					SDL_Rect dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

					// Perform visibility culling before rendering the tile.
					if (isVisibleOnScreen(dst, camPos, engine_.getScreenSize())) {
						SDL_Rect camAdjustedDst = {dst.x - camPos.x, dst.y - camPos.y, dst.w, dst.h};
						engine_.drawSpriteFromSheet(src, camAdjustedDst, spritesheet_);
					}
				}
			}
		}
	}

	Vec2d CalculateWeaponOffset(const Rotation &rotation)
	{
		if (rotation == Rotation::EAST)
			return {2, 6}; // EAST
		if (rotation == Rotation::SOUTH)
			return {-2, 2}; // SOUTH
		if (rotation == Rotation::WEST)
			return {-2, 6}; // WEST
		if (rotation == Rotation::NORTH)
			return {2, 2}; // NORTH

		return {0, 0};
	}

	void renderWeapon(ECSManager &ecs, const Entity &entity, const Vec2d &position, const Vec2d &camPos)
	{
		if (ecs.hasComponent<Rotatable>(entity)) {
			const auto &rotation = ecs.getComponent<Rotatable>(entity).rotation;
			Vec2d weaponOffset = CalculateWeaponOffset(rotation);

			auto flip = rotation == Rotation::EAST ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
			auto angle = rotation == Rotation::SOUTH || rotation == Rotation::NORTH ? 90 : 0;

			SDL_Rect wepSrc = {0, 0, 64, 32};
			SDL_Rect camAdjustedDstWep = {position.x + weaponOffset.x - camPos.x,
			                              position.y + weaponOffset.y - camPos.y, 16, 8};

			engine_.drawSpriteFromSheet(wepSrc, camAdjustedDstWep, weaponTexture, angle, nullptr, flip);
		}
	}

	void renderAlertnessLevel(const Vec2d &position, const AI &ai, const Vec2d &camPos)
	{ 
		const SDL_Rect dst{position.x - camPos.x, position.y - camPos.y - TILE_SIZE, TILE_SIZE, TILE_SIZE};
		std::string symbol;
		switch (ai.state) {
		case AIState::Unaware:
			symbol = ""; // Symbol for unaware state
			break;
		case AIState::Detecting:
			symbol = "?"; // Symbol for detecting state
			break;
		case AIState::Searching:
			symbol = "o.o"; // Symbol for searching state
			break;
		case AIState::Engaging:
			symbol = "!"; // Symbol for engaging state
			break;
		case AIState::Fleeing:
			symbol = "-!-"; // Symbol for fleeing state
			break;
		default:
			symbol = "#"; // Fallback symbol for undefined states
			break;
		}
		
		if (ai.state >= AIState::Detecting) {
			engine_.drawText(dst, symbol);
		}
	}

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
	SDL_Texture *spritesheet_;
	SDL_Texture *weaponTexture;
	double timeElapsed_ = 0; // elapsed time since last animation update
};
