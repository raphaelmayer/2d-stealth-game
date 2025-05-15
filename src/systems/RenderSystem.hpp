#include "../components/AI.hpp"
#include "../components/Animatable.hpp"
#include "../components/EquippedWeapon.hpp"
#include "../components/Health.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../engine/Engine.hpp"
#include "../items/WeaponDatabase.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Camera.hpp"
#include "../modules/SelectionManager.hpp"
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <cmath>
#include <easys/easys.hpp>
#include <functional>
#include <iostream>

// The RenderSystem is responsible for rendering the map and all entities with Renderable components.
// It performs visibility culling using the camera's position to avoid unnecessary rendering.
class RenderSystem final : public System {
  public:
	RenderSystem(Engine &engine, const MapManager &mapManager, const Camera &camera,
	             const SelectionManager &selectionManager)
	    : engine_(engine), mapManager_(mapManager), camera_(camera), selectionManager_(selectionManager)
	{
		textures.emplace(SPRITE_SHEET, engine_.loadTexture(SPRITE_SHEET));
		textures.emplace(M4A1, engine_.loadTexture(M4A1));
		textures.emplace(HERO_SHEET, engine_.loadTexture(HERO_SHEET));
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		Vec2f camPos = camera_.getPosition();
		float camZoom = camera_.getZoom();
		Vec2f screenSize = Utils::toFloat(engine_.getScreenSize()) / camZoom;
		Rectf camView{camPos.x, camPos.y, screenSize.x, screenSize.y};

		renderMap(camView, LayerID::BACKGROUND, LayerID::COSMETIC);

		const std::set<Easys::Entity> &entities = ecs.getEntities();
		for (const Easys::Entity &entity : entities) {
			if (ecs.hasComponent<Renderable>(entity) && ecs.hasComponent<Positionable>(entity)) {
				renderEntity(ecs, entity, camView);
			}
		}

		renderMap(camView, LayerID::FOREGROUND);

		renderSelectedEntities(ecs);
		renderSelectionRectangle();
	}

  private:
	void renderMap(const Rectf &camView, const LayerID start, const LayerID end = LayerID::NUM_LAYERS) const
	{
		const LevelMap &map = mapManager_.getLevelMap();

		// Calculate the range of visible tiles to selectively render based on the camera's position.
		int startX = std::max(0, static_cast<int>(camView.x / TILE_SIZE));
		int startY = std::max(0, static_cast<int>(camView.y / TILE_SIZE));
		int endX = std::min(map.getWidth(), static_cast<int>((camView.x + camView.w) / TILE_SIZE) + 1);
		int endY = std::min(map.getHeight(), static_cast<int>((camView.y + camView.h) / TILE_SIZE) + 1);

		for (int i = static_cast<int>(start); i < static_cast<int>(end); i++) {
			// for (auto layer : map.getLayers()) {
			auto layer = map.getLayers()[i];
			for (int y = startY; y < endY; y++) {
				for (int x = startX; x < endX; x++) {
					const int tileIndex = Utils::to1d({x, y}, map.getWidth());
					const int tileid = layer[tileIndex] - 1;
					const Vec2i srcPos = Utils::to2d(tileid, TILESET_COLUMNS) * TILE_SIZE;
					const Recti src = {srcPos.x, srcPos.y, TILE_SIZE, TILE_SIZE};
					const Recti dst = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
					const Rectf camAdjustedDst = camera_.rectToScreen(dst);
					engine_.drawTexture(getSpritesheet(SPRITE_SHEET), src, camAdjustedDst);
				}
			}
		}
	}

	bool isVisibleOnScreen(const Rectf &dst, const Rectf &camView) const
	{
		const float leftBound = camView.x - TILE_SIZE;
		const float rightBound = camView.x + camView.w;
		const float topBound = camView.y - TILE_SIZE;
		const float bottomBound = camView.y + camView.h;

		return dst.x >= leftBound && dst.x < rightBound && dst.y >= topBound && dst.y < bottomBound;
	}

	const Texture &getSpritesheet(const std::string filename) const
	{
		return textures.at(filename);
	}

	// Adjust sprite draw position so it aligns with the entity's logical world position.
	// Since sprites are drawn from the top-left corner, but positions represent ground contact points,
	// we offset vertically by (size.y - TILE_SIZE) to anchor the sprite's feet correctly.
	Vec2f offsetSpritePositionBySize(const Vec2f &pos, const Vec2f &size) const
	{
		if (size.y >= TILE_SIZE) {
			return pos - (size - TILE_SIZE);
		}
		return pos;
	}

	void renderEntity(Easys::ECS &ecs, Easys::Entity entity, const Rectf &camView) const
	{
		auto &position = ecs.getComponent<Positionable>(entity).position;
		auto &renderable = ecs.getComponent<Renderable>(entity);
		Vec2i sourcePosition = renderable.sourcePosition;
		Vec2i size = renderable.sourceSize;
		Vec2f targetSize = Utils::toFloat(renderable.targetSize);

		if (ecs.hasComponent<Rotatable>(entity)) {
			const auto rotation = ecs.getComponent<Rotatable>(entity).rotation;
			// due to how the spritesheet and the Rotation enum are laid out
			// this corresponds to the sprite for the current direction
			sourcePosition.x = rotation * 2 * size.x;
		}

		Recti src = {sourcePosition.x, sourcePosition.y, size.x, size.y};
		Vec2f sizeAdjusted = offsetSpritePositionBySize(position, targetSize);
		Rectf dst = {position.x, sizeAdjusted.y, targetSize.x, targetSize.y};

		// Perform visibility culling before rendering the entity.
		if (isVisibleOnScreen(dst, camView)) {
			const Texture &spritesheet = getSpritesheet(renderable.filename);
			Rectf camAdjustedDst = camera_.rectToScreen(dst);
			engine_.drawTexture(spritesheet, src, camAdjustedDst);

			if (ecs.hasComponent<AI>(entity)) {
				const AI &ai = ecs.getComponent<AI>(entity);
				renderAlertnessLevel(position, ai);
			}

			if (ecs.hasComponent<Health>(entity)) {
				const Health &health = ecs.getComponent<Health>(entity);
				renderHealthbar(position, health);
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

	void renderHealthbar(const Vec2f &position, const Health &health) const
	{
		if (health.health <= 0) {
			return;
		}

		float fillPercent = static_cast<float>(health.health) / static_cast<float>(health.maxHealth);
		// renderLoadingBar(Rectf{position.x, position.y, TILE_SIZE, 8}, fillPercent);
		Vec2f dst = camera_.vecToScreen(position);
		engine_.drawLine(dst, {dst.x + TILE_SIZE * camera_.getZoom() * fillPercent, dst.y}, {255, 120, 80, 255});
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

	// TODO: It is debatable if these functions should be here or in the UIRenderSystem.
	void renderSelectionRectangle()
	{
		auto start = selectionManager_.getStart();
		auto end = selectionManager_.getEnd();

		if (start == end) {
			return;
		}

		const Rectf selectionRectWorld = Utils::vectorsToRectangle(start, end);
		const Rectf selectionRectScreen = camera_.rectToScreen(selectionRectWorld);
		engine_.enableAlphaBlending();
		engine_.fillRectangle(selectionRectScreen, {66, 135, 245, 50});
		engine_.drawRectangle(selectionRectScreen, {66, 135, 245, 255});
		engine_.disableAlphaBlending();
	}

	void renderSelectedEntities(const Easys::ECS &ecs)
	{
		auto selection = selectionManager_.getSelection();

		if (selection.empty()) {
			return;
		}

		for (const Easys::Entity &e : selection) {
			if (ecs.hasComponent<Positionable>(e)) {
				auto pos = ecs.getComponent<Positionable>(e).position;
				const Rectf rect = {pos.x, pos.y, TILE_SIZE, TILE_SIZE};
				const Rectf dst = camera_.rectToScreen(rect);
				engine_.drawRectangle(dst, {66, 135, 245, 255});
			}
		}
	}

	Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
	const SelectionManager &selectionManager_;

	// we do not have a dedicated resource manager as of now, so we load textures here in the constructor and store them
	// in this map. we index textures by their respective file paths.
	std::unordered_map<std::string, Texture> textures;
};
