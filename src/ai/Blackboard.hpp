#pragma once

#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"

struct Blackboard {
	ECSManager &ecs;
	Entity entity;

	// waiting related
	double accumulator = 0;
	double deltaTime = 0;
	double timeout = 1;

	Blackboard(ECSManager &ecs_, Entity entity_) 
		: ecs(ecs_), entity(entity_) {}
};