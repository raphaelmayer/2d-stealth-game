#pragma once

#include "../constants.hpp"
#include "../engine/types/Vec2i.hpp"
#include <vector>

struct PatrolPoint {
	Vec2i position;
	Rotation rotation;
	double duration;

	// Maybe add field for special behavior, like triggering an animation or talking to an NPC.
	// Probably better handled within th BT.

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(position, rotation, duration);
	}
};

// This component is responsible for patrolling behavior. Every moving AI entity will probably have such a component, 
// because it can also be used to define a point to return to. And if an entity should just stand still and look around,
// this is also possible with this. 
struct Patrol {
	std::vector<PatrolPoint> waypoints = {};	// Predefined patrol route
	int patrolIndex = 0;						// Current position in the patrol path

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(waypoints, patrolIndex);
	}
};