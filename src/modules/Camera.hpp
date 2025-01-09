#pragma once

#include "../engine/Vec2d.hpp"
#include "../engine/Vf2d.hpp"
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

	// TODO: We probably want a method like this but probably as centerAround(focusPoint)
	void update(Vec2d focusPoint)
	{
		// Vec2d visibleArea = screenSize / zoom;
		// position = focusPoint - visibleArea / 2;
	}

	Vf2d getPosition() const { return position; }

	float getZoom() const { return zoom; }

	void zoomIn()
	{
		Vf2d visibleAreaBefore = screenSize / zoom;
		zoom += zoomStep;
		Vf2d visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	void zoomOut()
	{
		Vf2d visibleAreaBefore = screenSize / zoom;
		zoom -= zoomStep;
		Vf2d visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	void move(const CamDirection &direction)
	{
		position += dirv[static_cast<int>(direction)];
	}

  private:
	Vf2d screenSize;
	Vf2d position = {0.0f, 0.0f};
	float zoom = 1.0f;
	float zoomStep = 0.02f;
	std::vector<Vf2d> dirv = {{0, -4}, {4, 0}, {0, 4}, {-4, 0}}; // aligns with CamDirection enum
};
