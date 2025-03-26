#pragma once

// This component marks an entity as "tombstoned," meaning it is no longer active and will be removed soon.
// While we currently don't store any data here, we could use this component to hold relevant metadata like for example the position.
// The main advantage of this approach is that we can strip other components from tombstoned entities,
// keeping component storage minimal and avoiding unnecessary iterations over inactive entities / components.
struct Tombstone {
	/* data */

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(nullptr);
	}
};
