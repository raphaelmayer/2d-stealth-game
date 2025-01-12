#pragma once

#include "../engine/Vec2i.hpp"
#include "../engine/Vec2f.hpp"
#include <vector>

enum class CamDirection { UP = 0, RIGHT, DOWN, LEFT };

/*
 * A simple top-down camera designed for 2D games.
 * This camera keeps a specified focus point (e.g., the player) at the center of the view at all times.
 * The camera's position represents the top-left corner of the visible area in world coordinates.
 * The visible area is determined by the screen size, which is specified during initialization.
 * 
 * TODO: This comment is not true anymore. This camera can do a lot more, most notably zoom and move independently.
 */
class Camera {
  public:
	Camera(int width, int height) { screenSize = {(float)width, (float)height}; }
	
	void focus(Vec2i focusPoint) { 
		Vec2f p{focusPoint.x, focusPoint.y};
		focus(p);
	}

	void focus(Vec2f focusPoint)
	{
		Vec2f visibleArea = screenSize / zoom;
		position = focusPoint - visibleArea / 2;
	}

	Vec2f getPosition() const { return position; }

	float getZoom() const { return zoom; }

	void zoomIn()
	{
		Vec2f visibleAreaBefore = screenSize / zoom;
		zoom += zoomStep;
		Vec2f visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	void zoomOut()
	{
		Vec2f visibleAreaBefore = screenSize / zoom;
		zoom -= zoomStep;
		Vec2f visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	void move(const CamDirection &direction)
	{
		position += dirv[static_cast<int>(direction)];
	}

  private:
	Vec2f screenSize;
	Vec2f position = {0.0f, 0.0f};
	float zoom = 1.0f;
	float zoomStep = 0.02f;
	std::vector<Vec2f> dirv = {{0, -4}, {4, 0}, {0, 4}, {-4, 0}}; // aligns with CamDirection enum
};
