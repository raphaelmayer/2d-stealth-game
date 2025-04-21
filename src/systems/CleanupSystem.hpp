#include "../components/Tombstone.hpp"
#include "System.hpp"
#include <easys/easys.hpp>

// Handles the removal of entities marked for deletion by looking for a tombstone component.
class CleanupSystem : System {
  public:
	CleanupSystem() = default;

	void update(Easys::ECS &ecs, double deltaTime)
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