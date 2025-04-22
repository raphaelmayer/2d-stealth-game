#pragma once

#include <easys/easys.hpp>
#include <vector>

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