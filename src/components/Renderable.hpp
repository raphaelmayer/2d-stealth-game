#pragma once

#include "../engine/types/Vec2i.hpp"
#include <string>

// This component is used to connect entities to a sprite. An entity needs a Renderable component
// and a Transform component for the RenderSystem to be able to be render them correctly.
struct Renderable {
	std::string filename; // TODO: currently used to know which spritesheet to draw from. replace with some kind of ID
	                      // as soon as we have a asset loader/manager implemented.
	Vec2i sourcePosition; // coords of sprite on spritesheet
	Vec2i sourceSize;     // size of sprite on spritesheet
	Vec2i targetSize;     // scaled size of sprite when rendering
	//Vec2i offset{0, 0};   // currently not used, since we can offset by targetSize.

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(sourcePosition, sourceSize, targetSize);
	}
};
