#pragma once

// Currently a weapon component implies the entity is a shootable weapon, i.e. a gun. We might want to implement a more
// fine-grained system at some point.

// This struct could also be the type in the weapons database and the weapon component just holds the weapon id. 

struct Weapon {
	// // Some probably metadata is required or weapon as entity but feels unnecessary, when not on map.
	// std::string name;

	float velocity = 0;
	float range = 0;

	// We need some kind of randomness when firing weapons. Especially at range / out of optimal range.
	float dispersion = 0;
	float accuracy = 0; // different acc values for different ranges?

	int damage;         // if we don't do different ammunition
	int warmUp;         // Could be nice to simulate handling, at least when aiming
	int cooldown;       // or firerate?
	int stoppingPower;  // for staggering enemies when hit

	/*
	    Depending on how deep we want to go, we can choose a couple of approaches. Some ideas:
	    1. int cooldown, where a weapon has to wait to fire a new shot.
	    2. float firerate, where we use an actual firerate to calculate the interval of bullets. It
	    sounds unnecessarily complicated for our case.
	*/
};