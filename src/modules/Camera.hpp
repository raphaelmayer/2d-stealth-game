#pragma once

#include "../engine/Vec2d.hpp"

/*
 * A simple top-down camera designed for 2D games.
 * This camera keeps a specified focus point (e.g., the player) at the center of the view at all times.
 * The camera's position represents the top-left corner of the visible area in world coordinates.
 * The visible area is determined by the screen size, which is specified during initialization.
 */
class Camera {
  public:
	Camera(int width, int height) { screenSize = {width, height}; }

	void update(Vec2d focusPoint)
	{
		//Vec2d visibleArea = screenSize / zoom;
		//position = focusPoint - visibleArea / 2;
	}

	Vec2d getPosition() const { return position; }

	void setPosition(Vec2d value) { position = value; }

	float getZoom() const { return zoom; }

	void setZoom(float value) { zoom = value; }

  private:
	Vec2d screenSize;
	Vec2d position = {0, 0};
	float zoom = 1.0f;
};
