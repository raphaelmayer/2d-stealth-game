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
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <cmath>
#include <functional>
#include <iostream>

constexpr int TILESET_COLUMNS = 9; // TODO: Read from actual tileset data

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
		Vec2f screenSize = Utils::toFloat(engine_.getScreenSize()) / camZoom;
		Rectf camView{camPos.x, camPos.y, screenSize.x, screenSize.y};

		renderMap(camView);

		const std::set<Entity> &entities = ecs.getEntities();
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Renderable>(entity) && ecs.hasComponent<Positionable>(entity)) {
				renderEntity(ecs, entity, camView);
			}
		}
	}

  private:
	void renderMap(const Rectf &camView) const
	{
		const LevelMap &map = mapManager_.getLevelMap();

		// Calculate the range of visible tiles to selectively render based on the camera's position.
		int startX = std::max(0, static_cast<int>(camView.x / TILE_SIZE));
		int startY = std::max(0, static_cast<int>(camView.y / TILE_SIZE));
		int endX = std::min(map.getWidth(), static_cast<int>((camView.x + camView.w) / TILE_SIZE) + 1);
		int endY = std::min(map.getHeight(), static_cast<int>((camView.y + camView.h) / TILE_SIZE) + 1);

		for (auto layer : map.getLayers()) {
			for (int y = startY; y < endY; y++) {
				for (int x = startX; x < endX; x++) {
					const int tileIndex = Utils::to1d({x, y}, map.getWidth());
					const int tileid = layer[tileIndex] - 1;
					const Vec2i srcPos = Utils::to2d(tileid, TILESET_COLUMNS) * TILE_SIZE;
					const Recti src = {srcPos.x, srcPos.y, TILE_SIZE, TILE_SIZE};
					const Recti dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
					const Rectf camAdjustedDst = camera_.rectToScreen(dst);
					engine_.drawTexture(spritesheet_, src, camAdjustedDst);
				}
			}
		}
	}

	bool isVisibleOnScreen(const Recti &dst, const Rectf &camView) const
	{
		const float leftBound = camView.x - TILE_SIZE;
		const float rightBound = camView.x + camView.w;
		const float topBound = camView.y - TILE_SIZE;
		const float bottomBound = camView.y + camView.h;

		return dst.x >= leftBound && dst.x < rightBound && dst.y >= topBound && dst.y < bottomBound;
	}

	void renderEntity(ECSManager &ecs, Entity entity, const Rectf &camView) const
	{
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

		Recti src = {spriteSrc.x, spriteSrc.y, size.x, size.y};
		Recti dst = {position.x, position.y + offset_y, size.x, size.y};

		// Perform visibility culling before rendering the entity.
		if (isVisibleOnScreen(dst, camView)) {
			Rectf camAdjustedDst = camera_.rectToScreen(dst);
			engine_.drawTexture(spritesheet_, src, camAdjustedDst);

			if (ecs.hasComponent<AI>(entity)) {
				const AI &ai = ecs.getComponent<AI>(entity);
				renderAlertnessLevel(position, ai);
			}

			if (ecs.hasComponent<EquippedWeapon>(entity)) {
				const EquippedWeapon &ew = ecs.getComponent<EquippedWeapon>(entity);
				// renderWeapon(ecs, entity, position, ew); // TODO
				renderWarmupVisual(position, ew);
				renderReloadVisual(position, ew);
			}
		}
	}

	void renderAlertnessLevel(const Vec2f &position, const AI &ai) const
	{
		const Recti dst = {position.x + (TILE_SIZE / 4), position.y - TILE_SIZE, TILE_SIZE, TILE_SIZE};
		const Rectf camAdjustedDst = camera_.rectToScreen(dst);
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

	void renderDetectionVisual(const Vec2f &position, const AI &ai) const
	{
		const float maxTime = ai.detectionThreshold;
		const float fillPercent = ai.detectionTime / maxTime;

		renderLoadingBar(Rectf{position.x, position.y, TILE_SIZE, 4}, fillPercent);
	}

	void renderWarmupVisual(const Vec2f &position, const EquippedWeapon &ew) const
	{
		const WeaponMetadata wdata = WeaponDatabase::getInstance().get(ew.weaponId);

		if (ew.warmupAccumulator == 0 || ew.warmupAccumulator >= wdata.warmup)
			return;

		const float maxTime = wdata.warmup;
		const float fillPercent = ew.warmupAccumulator / maxTime;
		const Rectf dst = camera_.rectToScreen(Rectf{position.x, position.y - TILE_SIZE, TILE_SIZE * 2, TILE_SIZE / 2});

		engine_.drawText(dst, "WUP");
		renderLoadingBar(Rectf{position.x, position.y, TILE_SIZE, 4}, fillPercent);
	}

	void renderReloadVisual(const Vec2f &position, const EquippedWeapon &ew) const
	{
		auto wdata = WeaponDatabase::getInstance().get(ew.weaponId);

		if (ew.reloadTimeAccumulator == 0 || ew.reloadTimeAccumulator >= wdata.reloadTime)
			return;

		float maxTime = wdata.reloadTime;
		float fillPercent = ew.reloadTimeAccumulator / maxTime;
		Rectf dst = camera_.rectToScreen(Rectf{position.x, position.y - TILE_SIZE, TILE_SIZE * 2, TILE_SIZE / 2});

		engine_.drawText(dst, "RLD");
		renderLoadingBar(Rectf{position.x, position.y, TILE_SIZE, 4}, fillPercent);
	}

	void renderLoadingBar(const Rectf &rect, const float fillPercent) const
	{
		const float borderThickness = 1.0f;
		Rectf dstBorder = rect;
		Rectf dst = rect;
		dst.x = dst.x + borderThickness;
		dst.y = dst.y + borderThickness;
		dst.w = (dst.w + -2 * borderThickness) * fillPercent;
		dst.h = dst.h + -2 * borderThickness;

		dstBorder = camera_.rectToScreen(dstBorder);
		dst = camera_.rectToScreen(dst);

		engine_.drawRectangle(dstBorder, {255, 255, 255, 255});
		engine_.fillRectangle(dst, {50, 168, 82, 255});
	}

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
	const Texture spritesheet_;
	const Texture weaponTexture;
};
