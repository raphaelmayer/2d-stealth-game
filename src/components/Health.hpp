#pragma once

// Any damageable entity has a health component.
struct Health {
	int health = 100;
	int maxHealth = 100;
};