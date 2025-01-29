#include "../components/AI.hpp"
#include "../components/Animatable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Camera.hpp"
#include "System.hpp"
#include <cmath>
#include <functional>
#include <iostream>

// The RenderSystem is responsible for rendering the map and all entities with Renderable components.
// It performs visibility culling using the camera's position to avoid unnecessary rendering.
class RenderSystem final : public System {
  public:
	RenderSystem(const Engine &engine, const MapManager &mapManager, const Camera &camera)
	    : engine_(engine), mapManager_(mapManager), camera_(camera), spritesheet_(engine_.loadTexture(SPRITE_SHEET)),
	      weaponTexture(engine_.loadTexture(M4A1))
	{
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		Vec2f camPos = camera_.getPosition();
		float camZoom = camera_.getZoom();

		drawMap(camPos, camZoom);

		const std::set<Entity> &entities = ecs.getEntities();
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Renderable>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity).position;
				auto &renderable = ecs.getComponent<Renderable>(entity);

				Vec2i spriteSrc = renderable.spriteSrc;
				Vec2i size = renderable.size;
				int offset_y = renderable.offset_y;

				if (ecs.hasComponent<Rotatable>(entity)) {
					const auto rotation = ecs.getComponent<Rotatable>(entity).rotation;
					// due to how the spritesheet and the Rotation enum are laid out
					// this corresponds to the sprite for the current direction
					spriteSrc.x = rotation * 2 * size.x;
				}

				if (ecs.hasComponent<Animatable>(entity)) {
					auto &animatable = ecs.getComponent<Animatable>(entity);
					animatable.timeElapsed += deltaTime;
					handleAnimation(ecs, entity, animatable, spriteSrc.y);
				}

				Recti src = {spriteSrc.x, spriteSrc.y, size.x, size.y};
				Recti dst = {position.x, position.y + offset_y, size.x, size.y};

				// Perform visibility culling before rendering the entity.
				if (isVisibleOnScreen(dst, camPos, engine_.getScreenSize() / camZoom)) {
					Rectf camAdjustedDst = camera_.rectToScreen(dst);
					engine_.drawTexture(spritesheet_, src, camAdjustedDst);

					// Currently testing: rendering weapons
					// Render the weapon if applicable
					// renderWeapon(ecs, entity, position, camPos);
					if (ecs.hasComponent<AI>(entity)) {
						const AI &ai = ecs.getComponent<AI>(entity);
						renderAlertnessLevel(position, ai, camPos, camZoom);
						// renderDetectionVisual(position, ai);
					}
				}
			}
		}
	}

  private:
	void handleAnimation(ECSManager &ecs, const Entity entity, Animatable &animatable, int &spriteSrcY)
	{
		if (animatable.timeElapsed > ANIMATION_UPDATE_RATE_IN_SECONDS) {
			animatable.timeElapsed = 0;
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

	bool isVisibleOnScreen(const Recti &dst, const Vec2f &cameraPosition, const Vec2i &screenSize)
	{
		return dst.x >= (cameraPosition.x - TILE_SIZE) && dst.x < (cameraPosition.x + screenSize.x)
		       && dst.y >= (cameraPosition.y - TILE_SIZE) && dst.y < (cameraPosition.y + screenSize.y);
	}

	void drawMap(const Vec2f &camPos, const float &zoom)
	{
		const LevelMap &map = mapManager_.getLevelMap();

		// Calculate the range of visible tiles to render based on the camera's position.
		Vec2i visibleArea = engine_.getScreenSize() / zoom;
		int startX = std::max(0, static_cast<int>(camPos.x / TILE_SIZE));
		int startY = std::max(0, static_cast<int>(camPos.y / TILE_SIZE));
		int endX = std::min(map.getWidth(), static_cast<int>((camPos.x + visibleArea.x) / TILE_SIZE) + 1);
		int endY = std::min(map.getHeight(), static_cast<int>((camPos.y + visibleArea.y) / TILE_SIZE) + 1);

		for (auto layer : map.getLayers()) {
			for (int y = startY; y < endY; y++) {
				for (int x = startX; x < endX; x++) {
					int tileIndex = Utils::to1d({x, y}, map.getWidth());
					int tileid = layer[tileIndex];
					Vec2i srcPos = Utils::to2d(tileid - 1, 9) * TILE_SIZE; // TODO: should read 9 from tileset width
					Recti src = {srcPos.x, srcPos.y, TILE_SIZE, TILE_SIZE};
					Recti dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

					// Perform visibility culling before rendering the tile.
					if (isVisibleOnScreen(dst, camPos, engine_.getScreenSize() / zoom)) {
						Rectf camAdjustedDst = camera_.rectToScreen(dst);
						engine_.drawTexture(spritesheet_, src, camAdjustedDst);
					}
				}
			}
		}
	}

	Vec2i CalculateWeaponOffset(const Rotation &rotation)
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

	void renderWeapon(ECSManager &ecs, const Entity &entity, const Vec2f &position, const Vec2f &camPos)
	{
		if (ecs.hasComponent<Rotatable>(entity)) {
			const auto &rotation = ecs.getComponent<Rotatable>(entity).rotation;
			Vec2i weaponOffset = CalculateWeaponOffset(rotation);

			auto flip = rotation == Rotation::EAST ? TextureFlip::NONE : TextureFlip::HORIZONTAL;
			auto angle = rotation == Rotation::SOUTH || rotation == Rotation::NORTH ? 90 : 0;

			Recti wepSrc = {0, 0, 64, 32};
			Recti camAdjustedDstWep = {position.x + weaponOffset.x - camPos.x, position.y + weaponOffset.y - camPos.y,
			                           16, 8};

			engine_.drawTexture(weaponTexture, wepSrc, camAdjustedDstWep, angle, {8, 4}, flip);
		}
	}

	void renderAlertnessLevel(const Vec2f &position, const AI &ai, const Vec2f &camPos, const float &camZoom)
	{
		const Recti dst = {position.x + (TILE_SIZE / 4), position.y - TILE_SIZE, TILE_SIZE, TILE_SIZE};
		Rectf camAdjustedDst = camera_.rectToScreen(dst);
		std::string symbol;
		switch (ai.state) {
		case AIState::Unaware:
			symbol = ""; // Symbol for unaware state
			break;
		case AIState::Detecting:
			symbol = "";                         // Symbol for detecting state
			renderDetectionVisual(position, ai); // No symbol, render bar instead
			break;
		case AIState::Searching:
			symbol = "?"; // Symbol for searching state
			break;
		case AIState::Engaging:
			symbol = "!"; // Symbol for engaging state
			break;
		case AIState::Fleeing:
			symbol = "@"; // Symbol for fleeing state
			break;
		default:
			symbol = "#"; // Fallback symbol for undefined states
			break;
		}

		if (ai.state > AIState::Detecting) {
			engine_.drawText(camAdjustedDst, symbol);
		}
	}

	void renderDetectionVisual(const Vec2f &position, const AI &ai)
	{
		float maxTime = 2.0f; // TODO: read from component, where detection time is do
		float fillPercent = ai.detectionTime / maxTime;
		Rectf dstBorder;
		dstBorder.x = static_cast<float>(position.x);
		dstBorder.y = static_cast<float>(position.y);
		dstBorder.w = static_cast<float>(TILE_SIZE);
		dstBorder.h = static_cast<float>(4);

		Rectf dst = dstBorder;
		float borderThickness = 1.0f;
		dst.x = dst.x + borderThickness;
		dst.y = dst.y + borderThickness;
		dst.w = (dst.w + -2 * borderThickness) * fillPercent;
		dst.h = dst.h + -2 * borderThickness;

		dstBorder = camera_.rectToScreen(dstBorder);
		dst = camera_.rectToScreen(dst);

		engine_.drawRectangle(Vec2f{dstBorder.x, dstBorder.y}, dstBorder.w, dstBorder.h, {255, 255, 255, 255});
		engine_.fillRectangle(Vec2f{dst.x, dst.y}, dst.w, dst.h, {50, 168, 82, 255});
	}

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
	const Texture spritesheet_;
	const Texture weaponTexture;
};
