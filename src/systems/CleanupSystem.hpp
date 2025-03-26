#include "../components/Tombstone.hpp"
#include "../ecs/ECSManager.hpp"
#include "System.hpp"

// Handles the removal of entities marked for deletion by looking for a tombstone component.
class CleanupSystem : System {
  public:
	CleanupSystem() = default;

	void update(ECSManager &ecs, double deltaTime)
	{
		const auto entities = ecs.getEntities();
		for (const auto &entity : entities) {
			if (ecs.hasComponent<Tombstone>(entity)) {
				ecs.removeEntity(entity);
			}
		}
	}

  private:
};