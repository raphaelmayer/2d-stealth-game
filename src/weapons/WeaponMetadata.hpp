#pragma once

using WeaponID = int;

struct WeaponMetadata {
	WeaponID id;
	std::string name;
	std::string description;

	float velocity = 0;
	float range = 0;

	// We need some kind of randomness when firing weapons. Especially at range / out of optimal range.
	float dispersion = 0;
	float accuracy = 0; // different acc values for different ranges?

	int damage; // if we don't do different ammunition
	int armorPenetration;
	int stoppingPower; // for staggering enemies when hit
	int blastRadius;   // tiles affected by impact

	int warmup;   // Could work nicely to simulate handling
	int firerate; // or cooldown?

	int magazineSize;

	/*
	    - Depending on how deep we want to go, we can choose a couple of approaches. Some ideas:
	        1. int cooldown, where a weapon has to wait to fire a new shot.
	        2. float firerate, where we use an actual firerate to calculate the interval of bullets.
	    - Do we want magazines? Different ammunition?
	    - Can weapons be modified?
	*/
};