#pragma once

#include <easys/easys.hpp>
#include <vector>

// This class is a "dumb" storage class for the selection area the player draws with his mouse and the selected
// entities. It might make sense to give it more responsibility in the future like holding the selection rectangle
// instead of vectors and calculate the selected entities itself. Currently we set the selected entities in InputSystem
// and recalculate the rectangle every frame in the RenderSystem.
class SelectionManager {
  public:
	const std::vector<Easys::Entity> &getSelection() const
	{
		return selection;
	}

	void setSelection(const std::vector<Easys::Entity> &newSelection)
	{
		selection = newSelection;
	}

	void setStart(const Vec2f &v)
	{
		start = v;
	}

	Vec2f getStart() const
	{
		return start;
	}

	void setEnd(const Vec2f &v)
	{
		end = v;
	}

	Vec2f getEnd() const
	{
		return end;
	}

  private:
	std::vector<Easys::Entity> selection;
	Vec2f start;
	Vec2f end;
};