#include "../components/AI.hpp"
#include "../components/Collider.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../modules/MapManager.hpp"
#include "System.hpp"
#include <cmath>
#include <set>

class PathfindingSystem final : public System {
  public:
	PathfindingSystem(const MapManager &mapManager) : mapManager_(mapManager) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity).position;
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);
				// TODO: Should not happen for every entity, but currently this is how we omit checking an entity
				// against itself.
				auto walkableView = populateWalkableView(ecs, entity, mapManager_.getWalkableMapView());

				if (ecs.hasComponent<AI>(entity)) {
					auto &ai = ecs.getComponent<AI>(entity);
					handleAIPathfinding(position, rigidBody, ai, walkableView);
				}
			}
		}
	}

  private:
	void handleAIPathfinding(Vec2d &position, RigidBody &rigidBody, AI &ai, auto walkableView)
	{
		if (ai.targetPosition != Vec2d{-1, -1} && ai.targetPosition != position) {
			// If path does not point to target position, calculate a new one
			if (ai.path.empty() || ai.targetPosition != ai.path[ai.path.size() - 1]) {
				std::cout << "Recalculating path. pathIndex=" << ai.pathIndex << "\n";
				ai.path = AStar::findPath(walkableView, position.toTileSize(), ai.targetPosition.toTileSize());
				for (auto &waypoint : ai.path) // A* works in tile space, so we transform back to pixel space.
					waypoint *= TILE_SIZE;
				ai.pathIndex = 0;
			}

			// If we reach an intermediate position on our path, increment to next path position.
			if (ai.path[ai.pathIndex] == position) {
				if (ai.pathIndex < ai.path.size() - 1) {
					ai.pathIndex += 1;
					rigidBody.endPosition = ai.path[ai.pathIndex];
					// resetCurrentMovementParams(position, rigidBody);
				}
			} else {
				rigidBody.endPosition = ai.path[ai.pathIndex];
			}
		}
	}

	// TODO: Replace auto type, as soon as we decided on how to define and share a view.
	// This function adds collidable entities to our walkable map view.
	auto populateWalkableView(ECSManager &ecs, Entity entity, auto walkableView)
	{
		for (const auto &other : ecs.getEntities()) {
			if (ecs.hasComponent<Collider>(other) && ecs.hasComponent<Positionable>(other) && entity != other) {
				const auto &otherPos = ecs.getComponent<Positionable>(other).position.toTileSize();
				walkableView[otherPos.y][otherPos.x] = 1;
			}
		}
		return walkableView;
	}

	MapManager mapManager_;
};
