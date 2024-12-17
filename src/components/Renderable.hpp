#pragma once

#include "../engine/Vec2d.hpp"

// This component is used to connect entities to a sprite. An entity needs a Renderable component
// and a Transform component for the RenderSystem to be able to be render them correctly.
struct Renderable {
	Vec2d spriteSrc; // coords of sprite on spritesheet
	Vec2d size;      // size of sprite
	int offset_y; // offset in vertical axis, necessary for the player sprites, because they have free space above their
	              // heads

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(spriteSrc, size, offset_y);
	}
};
