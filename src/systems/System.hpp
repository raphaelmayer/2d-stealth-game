#pragma once

class ECSManager;

// Systems contain a single method update, which gets called every frame.
// The user has to provide an implementation.
class System {
  public:
	virtual void update(ECSManager &ecs, const double deltaTime) = 0;
};