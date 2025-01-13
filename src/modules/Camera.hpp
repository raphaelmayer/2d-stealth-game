#pragma once

#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include <vector>

enum class CamDirection { UP = 0, RIGHT, DOWN, LEFT };

/*
 * A flexible 2D camera for top-down games.
 * This camera centers a focus point (e.g., the player) in the visible area while supporting smooth zooming, directional
 * movement, and screen-to-world coordinate transformations. The visible area is determined by the screen size and zoom
 * level, and the camera's position corresponds to the top-left corner of the view in world coordinates.
 */
class Camera {
  public:
	Camera(int width, int height) { screenSize = {static_cast<float>(width), static_cast<float>(height)}; }

	void focus(Vec2i focusPoint)
	{
		Vec2f p{static_cast<float>(focusPoint.x), static_cast<float>(focusPoint.y)};
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

	void move(const CamDirection &direction) { position += dirv[static_cast<int>(direction)]; }

	Rectf rectToScreen(const Recti &rect) const
	{
		const Rectf dst = {static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.w),
		                   static_cast<float>(rect.h)};
		return rectToScreen(dst);
	}

	Rectf rectToScreen(const Rectf &rect) const
	{
		Vec2f screenPos = (Vec2f{rect.x, rect.y} - position) * zoom;
		Vec2f size = Vec2f{rect.w, rect.h} * zoom;
		Rectf camAdjustedDst{screenPos.x, screenPos.y, size.x, size.y};
		return camAdjustedDst;
	}

  private:
	Vec2f screenSize;
	Vec2f position = {0.0f, 0.0f};
	float zoom = 1.0f;
	float zoomStep = 0.02f;
	std::vector<Vec2f> dirv = {{0, -4}, {4, 0}, {0, 4}, {-4, 0}}; // aligns with CamDirection enum
};
