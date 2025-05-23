#pragma once

#include "../components/Pathfinding.hpp"
#include "../components/Positionable.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Vision.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Vec2f.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Camera.hpp"
#include "System.hpp"
#include <easys/easys.hpp>

class DebugSystem : public System {
  public:
	DebugSystem(const Engine &engine, const MapManager &mapManager, const Camera &camera)
	    : engine_(engine), mapManager_(mapManager), camera_(camera)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		renderVisionDebug(ecs);
		renderPaths(ecs);
	}

  private:
	void renderVisionDebug(Easys::ECS &ecs) const
	{
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Vision>(entity)) {
				auto pos = ecs.getComponent<Positionable>(entity).position;
				auto vision = ecs.getComponent<Vision>(entity);

				// drawViewCone(ecs, entity);
				drawLinesOfSight(ecs, entity, vision.visibleAllies, {0, 255, 0, 255});
				drawLinesOfSight(ecs, entity, vision.visibleEnemies, {255, 120, 80, 255});
			}
		}
	}

	void drawLinesOfSight(Easys::ECS &ecs, const Easys::Entity &entity, const std::vector<Easys::Entity> &others,
	                      const ColorRGBA &color) const
	{
		const Vec2f &pos = ecs.getComponent<Positionable>(entity).position;
		for (const auto &visibleEntity : others) {
			auto visibleEntityPosition = ecs.getComponent<Positionable>(visibleEntity).position;
			engine_.drawLine(screenOffset(pos), screenOffset(visibleEntityPosition), color);
		}
	}

	void drawViewCone(Easys::ECS &ecs, const Easys::Entity &entity) const
	{
		const auto &pos = ecs.getComponent<Positionable>(entity).position;
		const auto &rot = ecs.getComponent<Rotatable>(entity).rotation;
		const auto &vision = ecs.getComponent<Vision>(entity);

		// Map Rotation to forward direction vectors
		Vec2f forwardDirection = Utils::rotationToVec2f(rot);

		// Calculate the left and right edges of the cone
		float halfAngleRad = (vision.angle / 2.0f) * ((float)M_PI / 180.0f);
		Vec2f leftEdge = rotateVector(forwardDirection, halfAngleRad);
		Vec2f rightEdge = rotateVector(forwardDirection, -halfAngleRad);
		float quarterAngleRad = (vision.angle / 2.0f / 2.0f) * ((float)M_PI / 180.0f);
		Vec2f leftQEdge = rotateVector(forwardDirection, quarterAngleRad);
		Vec2f rightQEdge = rotateVector(forwardDirection, -quarterAngleRad);

		// Scale the edges by the vision range
		leftEdge = leftEdge * vision.range;
		rightEdge = rightEdge * vision.range;
		leftQEdge = leftQEdge * vision.range;
		rightQEdge = rightQEdge * vision.range;

		// Draw the cone edges
		engine_.drawLine(screenOffset(pos), screenOffset(pos + leftEdge), {80, 255, 255, 255});
		engine_.drawLine(screenOffset(pos), screenOffset(pos + rightEdge), {80, 255, 255, 255});
		engine_.drawLine(screenOffset(pos), screenOffset(pos + leftQEdge), {80, 255, 255, 255});
		engine_.drawLine(screenOffset(pos), screenOffset(pos + rightQEdge), {80, 255, 255, 255});
		engine_.drawLine(screenOffset(pos), screenOffset(pos + forwardDirection * vision.range), {80, 255, 255, 255});

		// TODO: draw the arc of the cone
	}

	Vec2f rotateVector(const Vec2f &vec, float angleRad) const
	{
		float cosAngle = std::cos(angleRad);
		float sinAngle = std::sin(angleRad);
		return {vec.x * cosAngle - vec.y * sinAngle, vec.x * sinAngle + vec.y * cosAngle};
	}

	void renderPaths(Easys::ECS &ecs) const
	{
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Pathfinding>(entity)) {
				const auto &pf = ecs.getComponent<Pathfinding>(entity);
				for (size_t i = pf.pathIndex; i < pf.path.size(); i++) {
					if (i + 1 < pf.path.size())
						engine_.drawLine(screenOffset(Utils::toFloat(pf.path[i])),
						                 screenOffset(Utils::toFloat(pf.path[i + 1])), {255, 255, 255, 255});
				}
				engine_.drawCircle(screenOffset(Utils::toFloat(pf.targetPosition)), (TILE_SIZE / 2) * camera_.getZoom(),
				                   {255, 255, 255, 255});
			}
		}
	}

	// Computes the offset to center objects within tiles and adjust for the camera's position.
	// Ensures lines and shapes are drawn relative to the tile grid, aligned to tile centers.
	Vec2f screenOffset(const Vec2f &position) const
	{
		Vec2f pos{position.x - camera_.getPosition().x, position.y - camera_.getPosition().y};
		return (pos)*camera_.getZoom() + float(TILE_SIZE / 2) * camera_.getZoom();
	}

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
};