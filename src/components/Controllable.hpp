#pragma once

#include <cstdint>

struct Controllable {
	// InputSystem looks for this component when deciding which entity to update

	template <class Archive>
	void serialize(Archive &archive)
	{
		// archive();
	}
};
