#pragma once

#include "../ecs/Entity.hpp"

// This is a temporary component an entity has, while it actively tries to engage with an entity.
struct Target {
	Entity entity;
};