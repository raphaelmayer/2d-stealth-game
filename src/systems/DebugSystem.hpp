#pragma once

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

	void update(ECSManager &ecs, const double deltaTime) override { 
		renderVisionDebug(ecs);
	}

  private:
	void renderVisionDebug(ECSManager &ecs)
	{
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Vision>(entity)) {
				auto pos = ecs.getComponent<Positionable>(entity).position;
				for (const auto &visibleEntity : ecs.getComponent<Vision>(entity).visibleEntities) {
					auto visibleEntityPosition = ecs.getComponent<Positionable>(visibleEntity).position;
					engine_.drawRectangle(visibleEntityPosition - camera_.getPosition(), TILE_SIZE, TILE_SIZE,
					                      {255, 255, 255, 255});
					engine_.drawLine(pos - camera_.getPosition(), visibleEntityPosition - camera_.getPosition(),
					               {255, 255, 255, 255});
					engine_.drawLine(pos - camera_.getPosition() + Vec2d{TILE_SIZE, 0},
					               visibleEntityPosition - camera_.getPosition() + Vec2d{TILE_SIZE, 0},
					               {255, 255, 255, 255});
					engine_.drawLine(pos - camera_.getPosition() + Vec2d{0, TILE_SIZE},
					               visibleEntityPosition - camera_.getPosition() + Vec2d{0, TILE_SIZE},
					               {255, 255, 255, 255});
					engine_.drawLine(pos - camera_.getPosition() + Vec2d{TILE_SIZE, TILE_SIZE},
					               visibleEntityPosition - camera_.getPosition() + Vec2d{TILE_SIZE, TILE_SIZE},
					               {255, 255, 255, 255});
				}
				drawViewCone(ecs, entity);
			}
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

		// Scale the edges by the vision range
		leftEdge = leftEdge * vision.range;
		rightEdge = rightEdge * vision.range;

		// Draw the cone edges
		engine_.drawLine(pos - camera_.getPosition() + TILE_SIZE / 2,
		                 (pos + leftEdge.toVec2d()) - camera_.getPosition() + TILE_SIZE / 2, {0, 255, 0, 255});
		engine_.drawLine(pos - camera_.getPosition() + TILE_SIZE / 2,
		                 (pos + rightEdge.toVec2d()) - camera_.getPosition() + TILE_SIZE / 2, {0, 255, 0, 255});
		engine_.drawLine(pos - camera_.getPosition() + TILE_SIZE / 2,
		                 (pos + forwardDirection.toVec2d() * vision.range) - camera_.getPosition() + TILE_SIZE / 2, {0, 255, 0, 255});

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

	const Engine &engine_;
	const MapManager &mapManager_;
	const Camera &camera_;
};