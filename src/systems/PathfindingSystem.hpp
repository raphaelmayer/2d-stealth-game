#include "../components/AI.hpp"
#include "../components/Collider.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Utils.hpp"
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
	void handleAIPathfinding(Vec2f &position, RigidBody &rigidBody, AI &ai, auto walkableView)
	{
		if (ai.targetPosition != Vec2i{-1, -1} && ai.targetPosition != Utils::toInt(position)) {
			// TODO: We need to check, if targetPosition is reachable. If not, we could use a couple of strategies like
			// finding the nearest reachable tile.

			// If path does not point to target position, calculate a new one
			if (ai.path.empty() || ai.targetPosition != ai.path[ai.path.size() - 1]) {
				std::cout << "Recalculating path. pathIndex=" << ai.pathIndex << ", pos x: " << position.x
				          << ", pos y: " << position.y << ", tarpos x: " << position.x << ", tarpos y: " << position.y
				          << "\n";
				auto intpath =
				    AStar::findPath(walkableView, Utils::toTileSize(position), Utils::toTileSize(ai.targetPosition));
				ai.path = {};
				for (auto &waypoint : intpath) // A* works in tile space, so we transform back to pixel space.
					ai.path.push_back(waypoint * TILE_SIZE);
				ai.pathIndex = 0;
			}

			// If we reach an intermediate position on our path, increment to next path position.
			if (ai.path[ai.pathIndex] == Utils::toInt(position)) {
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
				const auto &otherPos = Utils::toTileSize(ecs.getComponent<Positionable>(other).position);
				walkableView[otherPos.y][otherPos.x] = 1;
			}
		}
		return walkableView;
	}

	MapManager mapManager_;
};
