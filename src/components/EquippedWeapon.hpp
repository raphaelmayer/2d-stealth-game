#pragma once

#include "../items/WeaponMetadata.hpp"

// This component represents the equipped weapon (i.e. an instance of this weapon)
// for a humanoid entity. It tracks both the weapon's identifier and its state (ammo count,
// warmup, fire rate, and reload timing).
struct EquippedWeapon {
	WeaponID weaponId = 0;

	int magazineSize = 0;              // tracks the amount of bullets in a magazine
	int maxMagazineSize = magazineSize; // we should read this from the wdb or rethink how we instantiate weapons

	float warmupAccumulator = 0;     // accumulator for the weapon's warmup phase
	float firerateAccumulator = 0;   // fire rate control enforcing a delay between shots
	float reloadTimeAccumulator = 0; // tracks the reload duration

	bool isTriggerHeld = false;
	bool isReloading = false; // flag is being set in firing system
};
