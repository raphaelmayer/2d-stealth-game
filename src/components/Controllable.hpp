#pragma once

#include <cstdint>

struct Controllable {
	// InputSystem looks for this component when deciding which entity to update
	uint32_t isInInteractionWith = 0; // can not be Entity for some reason
	bool isTryingToStartInteraction = false;
	bool isTryingToEndInteraction = false;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(isInInteractionWith, isTryingToStartInteraction, isTryingToEndInteraction);
	}
};
