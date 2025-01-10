#pragma once

#include "../components/AI.hpp"
#include "../components/Positionable.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2d.hpp"
#include "../engine/Vf2d.hpp"
#include "../modules/Camera.hpp"
#include "../modules/MapManager.hpp"
#include "System.hpp"

class DebugSystem : public System {
  public:
	DebugSystem(const Engine &engine, const MapManager &mapManager, const Camera &camera)
	    : engine_(engine), mapManager_(mapManager), camera_(camera)
	{
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		renderVisionDebug(ecs);
		renderPaths(ecs);
	}

  private:
	void renderVisionDebug(ECSManager &ecs)
	{
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Vision>(entity)) {
				auto pos = ecs.getComponent<Positionable>(entity).position;
				auto vision = ecs.getComponent<Vision>(entity);

				drawViewCone(ecs, entity);
				drawLinesOfSight(ecs, entity, vision.visibleAllies, {0, 255, 0, 255});
				drawLinesOfSight(ecs, entity, vision.visibleEnemies, {255, 120, 80, 255});
			}
		}
	}

	void drawLinesOfSight(ECSManager &ecs, const Entity &entity, const std::vector<Entity> &others,
	                      const ColorRGBA &color)
	{
		const Vec2d &pos = ecs.getComponent<Positionable>(entity).position;
		for (const auto &visibleEntity : others) {
			auto visibleEntityPosition = ecs.getComponent<Positionable>(visibleEntity).position;
			engine_.drawLine(offset(pos), offset(visibleEntityPosition), color);
		}
	}

	void drawViewCone(ECSManager &ecs, const Entity &entity)
	{
		const auto &pos = ecs.getComponent<Positionable>(entity).position;
		const auto &rot = ecs.getComponent<Rotatable>(entity).rotation;
		const auto &vision = ecs.getComponent<Vision>(entity);

		// Map Rotation to forward direction vectors
		Vf2d forwardDirection = rotationToVf2d(rot);

		// Calculate the left and right edges of the cone
		float halfAngleRad = (vision.angle / 2.0f) * (M_PI / 180.0f);
		Vf2d leftEdge = rotateVector(forwardDirection, halfAngleRad);
		Vf2d rightEdge = rotateVector(forwardDirection, -halfAngleRad);
		float quarterAngleRad = (vision.angle / 2.0f / 2.0f) * (M_PI / 180.0f);
		Vf2d leftQEdge = rotateVector(forwardDirection, quarterAngleRad);
		Vf2d rightQEdge = rotateVector(forwardDirection, -quarterAngleRad);

		// Scale the edges by the vision range
		leftEdge = leftEdge * vision.range;
		rightEdge = rightEdge * vision.range;
		leftQEdge = leftQEdge * vision.range;
		rightQEdge = rightQEdge * vision.range;

		// Draw the cone edges
		engine_.drawLine(offset(pos), offset((pos + leftEdge.toVec2d())), {80, 255, 255, 255});
		engine_.drawLine(offset(pos), offset((pos + rightEdge.toVec2d())), {80, 255, 255, 255});
		engine_.drawLine(offset(pos), offset((pos + leftQEdge.toVec2d())), {80, 255, 255, 255});
		engine_.drawLine(offset(pos), offset((pos + rightQEdge.toVec2d())), {80, 255, 255, 255});
		engine_.drawLine(offset(pos), offset((pos + forwardDirection.toVec2d() * vision.range)), {80, 255, 255, 255});

		// TODO: draw the arc of the cone
	}

	Vf2d rotateVector(const Vf2d &vec, float angleRad) const
	{
		float cosAngle = std::cos(angleRad);
		float sinAngle = std::sin(angleRad);
		return {vec.x * cosAngle - vec.y * sinAngle, vec.x * sinAngle + vec.y * cosAngle};
	}

	Vf2d rotationToVf2d(Rotation rotation) const
	{

		switch (rotation) {
		case NORTH:
			return {0, -1};
			break;
		case EAST:
			return {1, 0};
			break;
		case SOUTH:
			return {0, 1};
			break;
		case WEST:
			return {-1, 0};
			break;
		}
		return {0, 0};
	}

	void renderPaths(ECSManager &ecs)
	{
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<AI>(entity)) {
				const auto &ai = ecs.getComponent<AI>(entity);
				for (size_t i = ai.pathIndex; i < ai.path.size(); i++) {
					if (i + 1 < ai.path.size())
						engine_.drawLine(offset(ai.path[i]), offset(ai.path[i + 1]), {255, 255, 255, 255});
				}
				engine_.drawCircle(offset(ai.targetPosition), (TILE_SIZE / 2) * camera_.getZoom(),
				                   {255, 255, 255, 255});
			}
		}
	}

	// Computes the offset to center objects within tiles and adjust for the camera's position.
	// Ensures lines and shapes are drawn relative to the tile grid, aligned to tile centers.
	Vec2d offset(const Vec2d &position)
	{
		Vec2d pos{position.x - camera_.getPosition().x, position.y - camera_.getPosition().y};
		return (pos)*camera_.getZoom() + (TILE_SIZE / 2) * camera_.getZoom();
	}

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
};