#pragma once

#include "../constants.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Recti.hpp"
#include "../engine/types/Vec2i.hpp"
#include <vector>

// UIElement is a base class for text-based UI elements like menus and dialog boxes.
// It provides fundamental styling (borders, padding, color) of UI elements and an interface for derived classes.
class UIElement {
  public:
	UIElement(Engine &game) : game_(game) {}
	virtual ~UIElement() = default;

	virtual void handleInput() = 0;
	virtual void render() = 0;

  protected:
	void renderContainer(Vec2i position, int menuWidth, int menuHeight)
	{
		ColorRGBA BLACK = {0, 0, 0, 255};
		ColorRGBA WHITE = {255, 255, 255, 255};

		game_.setRenderScale({1, 1});

		// Fill the main menu rectangle with white color
		game_.fillRectangle(Recti{position.x, position.y, menuWidth, menuHeight}, WHITE);

		// Draw the outer border of the menu in black
		game_.drawRectangle(Recti{position.x, position.y, menuWidth, menuHeight}, BLACK);

		// Draw a slightly smaller inner border in black
		Vec2i innerPos1 = position + 1;
		Vec2i innerSize1 = Vec2i{menuWidth - 2, menuHeight - 2};
		game_.drawRectangle(Recti{innerPos1.x, innerPos1.y, innerSize1.x, innerSize1.y}, BLACK);

		// Draw a padded inner border in black
		Vec2i paddedPos = position + PADDING;
		Vec2i paddedSize = Vec2i{menuWidth, menuHeight} - 2 * PADDING;
		game_.drawRectangle(Recti{paddedPos.x, paddedPos.y, paddedSize.x, paddedSize.y}, BLACK);

		// Draw a smaller padded inner border in black
		Vec2i smallerPaddedPos = position + PADDING + 1;
		Vec2i smallerPaddedSize = Vec2i{menuWidth, menuHeight} - 2 * PADDING - 2;
		game_.drawRectangle(Recti{smallerPaddedPos.x, smallerPaddedPos.y, smallerPaddedSize.x, smallerPaddedSize.y},
		                    BLACK);

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