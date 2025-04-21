#pragma once

class Easys::ECS;

// Systems contain a single method update, which gets called every frame.
// The user has to provide an implementation.
class System {
  public:
	virtual void update(Easys::ECS &ecs, const double deltaTime) = 0;
};