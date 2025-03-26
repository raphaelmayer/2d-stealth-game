#pragma once

#include "../engine/types/Rectf.hpp"
#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include <vector>

enum class CamDirection { UP = 0, RIGHT, DOWN, LEFT };

/*
 * A flexible 2D camera for top-down games.
 * This camera supports smooth zooming, directional movement, and screen-to-world coordinate transformations. It can
 * also center around a focus point (e.g., the player) in the visible area.
 * The visible area is determined by the screen size and zoom level, and the camera's position corresponds to the
 * top-left corner of the view in world coordinates.
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

	void setPosition(const Vec2f &pos) { position = pos; }

	void move(const Vec2f &offset) { position += offset / zoom; }

	void move(const CamDirection &direction, float amount = DEFAULT_MOVESTEP)
	{
		move(dirv[static_cast<int>(direction)] * amount);
	}

	float getZoom() const { return zoom; }

	void setZoom(float amount) { zoom = amount; }

	void zoomIn(float zoomAmount = DEFAULT_ZOOMSTEP)
	{
		Vec2f visibleAreaBefore = screenSize / zoom;
		zoom += zoomAmount;
		Vec2f visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	void zoomOut(float zoomAmount = DEFAULT_ZOOMSTEP)
	{
		Vec2f visibleAreaBefore = screenSize / zoom;
		zoom -= zoomAmount;
		Vec2f visibleAreaAfter = screenSize / zoom;

		// Adjust position to keep the center consistent
		position += (visibleAreaBefore - visibleAreaAfter) / 2;
	}

	Vec2f vecToScreen(const Vec2i &vec) const
	{
		const Vec2f dst = {static_cast<float>(vec.x), static_cast<float>(vec.y)};
		return vecToScreen(dst);
	}

	Vec2f vecToScreen(const Vec2f &vec) const
	{
		Vec2f dst = (Vec2f{vec.x, vec.y} - position) * zoom;
		return dst;
	}

	Rectf rectToScreen(const Recti &rect) const
	{
		const Rectf dst = {static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.w),
		                   static_cast<float>(rect.h)};
		return rectToScreen(dst);
	}

	Rectf rectToScreen(const Rectf &rect) const
	{
		Vec2f screenPos = vecToScreen(Vec2f{rect.x, rect.y});
		Vec2f size = Vec2f{rect.w, rect.h} * zoom;
		Rectf camAdjustedDst{screenPos.x, screenPos.y, size.x, size.y};
		return camAdjustedDst;
	}

	Vec2f screenToWorld(const Vec2f &vec) const { return (vec / getZoom()) + getPosition(); }

  private:
	Vec2f screenSize;
	Vec2f position = {0.0f, 0.0f};
	float zoom = 1.0f;
	std::vector<Vec2f> dirv = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; // aligns with CamDirection enum

	static constexpr float DEFAULT_ZOOMSTEP = 0.02f;
	static constexpr float DEFAULT_MOVESTEP = 4.0f;
};
