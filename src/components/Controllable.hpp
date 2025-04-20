#pragma once

#include <cstdint>

// InputSystem looks for this component when deciding which entity to update
struct Controllable {

	template <class Archive>
	void serialize(Archive &archive)
	{
		// archive();
	}
};
