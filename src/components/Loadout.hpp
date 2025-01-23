#pragma once

// This is the equipment a character has currently equipped. Currently entities can also have an inventory, which might
// not even be something we need in our game. But we will see what and how we want todo things.

// These members will probably be of type WeaponId or similar and reference a central database for weapons, armor, etc.

struct Loadout {
	int mainHand;
	int offHand;
	int throwable;
};