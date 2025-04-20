#pragma once

#include <easys/easys.hpp>

// DamageEvent is not an actual event in the programming kind of sense. It just contains info about damage needing to be
// applied to an entity.
struct DamageEvent {
	Easys::Entity source = 0;
	int amount = 0;
};

// Temporary component assigned to entities that should receive damage. The damage system processes this component.
// To handle multiple damage sources in a single frame, we can accumulate "damage events" within this component.
// We could switch to an event-driven (queue-based) approach for more flexibility.
struct DamageBuffer {
	std::vector<DamageEvent> damageEvents;
};
