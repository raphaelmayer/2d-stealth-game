#pragma once

#include "../constants.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2i.hpp"
#include <vector>

// UIElement is a base class for text-based UI elements like menus and dialog boxes.
// It provides fundamental styling (borders, padding, color) of UI elements and an interface for derived classes.
class UIElement {
  public:
	UIElement(Engine &game) : game_(game) {}

	virtual void handleInput() = 0;
	virtual void render() = 0;

  protected:
	void renderContainer(Vec2i position, int menuWidth, int menuHeight)
	{
		ColorRGBA BLACK = {0, 0, 0, 0};
		ColorRGBA WHITE = {255, 255, 255, 0};

		game_.setRenderScale({1, 1});

		game_.fillRectangle(position, menuWidth, menuHeight, WHITE);
		game_.drawRectangle(position, menuWidth, menuHeight, BLACK);
		game_.drawRectangle(position + 1, menuWidth - 2, menuHeight - 2, BLACK);
		game_.drawRectangle(position + PADDING, menuWidth - 2 * PADDING, menuHeight - 2 * PADDING, BLACK);
		game_.drawRectangle(position + PADDING + 1, menuWidth - 2 * PADDING - 2, menuHeight - 2 * PADDING - 2, BLACK);

		// adjustable parameters
		int oRad = 5;   // outer circles radius
		int iRad = 3;   // inner circles radius
		int offset = 2; // circle offset

		// outer circles
		game_.fillCircle(position + offset, oRad, BLACK);                                // top left
		game_.fillCircle(position + Vec2i{menuWidth - offset - 1, offset}, oRad, BLACK); // top right
		game_.fillCircle(position + Vec2i{menuWidth - offset - 1, menuHeight - offset - 1}, oRad,
		                 BLACK);                                                          // bottom right
		game_.fillCircle(position + Vec2i{offset, menuHeight - offset - 1}, oRad, BLACK); // bottom left

		// inner circles
		game_.fillCircle(position + offset, iRad, WHITE);                                // top left
		game_.fillCircle(position + Vec2i{menuWidth - offset - 1, offset}, iRad, WHITE); // top right
		game_.fillCircle(position + Vec2i{menuWidth - offset - 1, menuHeight - offset - 1}, iRad,
		                 WHITE);                                                          // bottom right
		game_.fillCircle(position + Vec2i{offset, menuHeight - offset - 1}, iRad, WHITE); // bottom left

		game_.setRenderScale({PIXEL_SIZE, PIXEL_SIZE});
	}

  private:
	Engine &game_;
};