#pragma once

using WeaponID = int;

struct WeaponMetadata {
	WeaponID id;
	std::string name;
	std::string description;

	float velocity = 0;
	float range = 0;

	int damage = 0; // if we don't do different ammunition. otherwise we need a caliber member
	int armorPenetration = 0;
	int stoppingPower = 0; // for staggering enemies when hit
	int blastRadius = 0;   // tiles affected by impact

	float warmup = 0;   // secs
	float firerate = 0; // secs

	int magazineSize = 0;
	float reloadTime = 0; // secs

	float spread = 0;

	float accuracy = 0; // different acc values for different ranges?
	                    /*
	                        - Depending on how deep we want to go, we can choose a couple of approaches. Some ideas:
	                            1. int cooldown, where a weapon has to wait to fire a new shot.
	                            2. float firerate, where we use an actual firerate to calculate the interval of bullets.
	                        - Do we want magazines? Different ammunition?
	                        - Can weapons be modified?
	                    */
};