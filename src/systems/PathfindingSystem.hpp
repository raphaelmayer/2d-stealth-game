#include "../components/Collider.hpp"
#include "../components/Pathfinding.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <cmath>
#include <easys/easys.hpp>
#include <set>

class PathfindingSystem final : public System {
  public:
	PathfindingSystem(const MapManager &mapManager) : mapManager_(mapManager)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		const std::set<Easys::Entity> &entities = ecs.getEntities();

		for (const Easys::Entity &entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity).position;
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);
				// TODO: Should not happen for every entity, but currently this is how we omit checking an entity
				// against itself.
				// auto walkableView = populateWalkableView(ecs, entity, mapManager_.getWalkableMapView());
				auto walkableView = mapManager_.getWalkableMapView();

				if (ecs.hasComponent<Pathfinding>(entity)) {
					Collider &collider = ecs.getComponent<Collider>(entity);
					if (collider.didCollide) {
						Vec2f lastCollideePosition = Utils::toFloat(Utils::toTileSize(collider.lastCollisionPosition));
						walkableView[lastCollideePosition.y][lastCollideePosition.x] = 1;
						collider.didCollide = false;
					}
					auto &pf = ecs.getComponent<Pathfinding>(entity);
					handleAIPathfinding(position, rigidBody, pf, walkableView);
				}
			}
		}
	}

  private:
	void handleAIPathfinding(Vec2f &position, RigidBody &rigidBody, Pathfinding &pf, auto walkableView) const
	{
		if (pf.targetPosition != Vec2i{-1, -1} && pf.targetPosition != Utils::toInt(position)) {
			// TODO: We need to check, if targetPosition is reachable. If not, we could use a couple of strategies like
			// finding the nearest reachable tile.

			// If path does not point to target position, calculate a new one
			if (pf.path.empty() || pf.targetPosition != pf.path[pf.path.size() - 1]) {
				std::cout << "Recalculating path. pathIndex=" << pf.pathIndex << ", pos x: " << position.x
				          << ", pos y: " << position.y << ", tarpos x: " << position.x << ", tarpos y: " << position.y
				          << "\n";
				auto intpath =
				    AStar::findPath(walkableView, Utils::toTileSize(position), Utils::toTileSize(pf.targetPosition));
				pf.path = {};
				for (auto &waypoint : intpath) // A* works in tile space, so we transform back to pixel space.
					pf.path.push_back(waypoint * TILE_SIZE);
				pf.pathIndex = 0;
			}

			// If we reach an intermediate position on our path, increment to next path position.
			if (pf.path[pf.pathIndex] == Utils::toInt(position)) {
				if (pf.pathIndex < pf.path.size() - 1) {
					pf.pathIndex += 1;
					rigidBody.nextPosition = pf.path[pf.pathIndex];
					// resetCurrentMovementParams(position, rigidBody);
				}
			} else {
				rigidBody.nextPosition = pf.path[pf.pathIndex];
			}
		}
	}

	// TODO: Replace auto type, as soon as we decided on how to define and share a view.
	// This function adds collidable entities to our walkable map view.
	auto populateWalkableView(Easys::ECS &ecs, Easys::Entity entity, auto walkableView) const
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
