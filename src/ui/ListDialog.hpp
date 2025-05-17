#pragma once

#include "../constants.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Recti.hpp"
#include "../engine/types/Vec2i.hpp"
#include "ListItem.hpp"
#include "UIElement.hpp"
#include <algorithm>
#include <vector>

// A generic UI component for displaying and navigating through a list of items.
// The class supports scrollable lists, automatically adjusting to display a subset of items when
// the total count exceeds the defined maximum visible items.
class ListDialog : public UIElement {
  public:
	ListDialog(Engine &game, std::vector<ListItem> items, Vec2i position, int menuWidth, int maxVisibleItems = 5)
	    : UIElement(game), game_(game), items_(items)
	{
		initializeMenu(position, menuWidth, maxVisibleItems);
	}
	ListDialog(Engine &game, Vec2i position, int menuWidth, int maxVisibleItems = 5) : UIElement(game), game_(game)
	{
		initializeMenu(position, menuWidth, maxVisibleItems);
	}

	void handleInput()
	{
		if (game_.input().getKeyState(SDL_SCANCODE_S).pressed)
			navigateDown();

		if (game_.input().getKeyState(SDL_SCANCODE_W).pressed)
			navigateUp();

		if (game_.input().getKeyState(SDL_SCANCODE_RETURN).pressed)
			select();
	}

	void render()
	{
		renderContainer(position_, menuWidth_, menuHeight_);
		renderListItems();
	}

	void resetMenuPosition()
	{
		index_ = 0;
		top_ = 0;
	}

	void setItems(std::vector<ListItem> items)
	{
		resetMenuPosition();
		items_ = items;
		menuHeight_ = calculateMenuHeight();
	}

	int getIndex() const { return index_; }
	size_t size() const { return items_.size(); }

  private:
	Engine &game_;
	std::vector<ListItem> items_;
	int index_ = 0, top_ = 0, maxVisibleItems_;

	Vec2i position_;
	int menuWidth_, menuHeight_;
	int itemWidth_, itemHeight_;

	void navigateDown()
	{
		if (index_ < items_.size() - 1) {
			index_++;
			// Adjust top_ when index_ is one less than the bottom of the current window
			if (index_ > top_ + maxVisibleItems_ - 2 && top_ < items_.size() - maxVisibleItems_) {
				top_++;
			}
		} else {
			index_ = 0;
			top_ = 0;
		}
	}

	void navigateUp()
	{
		if (index_ > 0) {
			index_--;
			// Adjust top_ when index_ is two less than the current top
			if (index_ < top_ + 1 && top_ > 0) {
				top_--;
			}
		} else {
			index_ = static_cast<int>(items_.size()) - 1;
			top_ = std::max(0, static_cast<int>(items_.size()) - maxVisibleItems_);
		}
	}

	void select()
	{
		if (index_ >= 0 && index_ < items_.size()) {
			items_[index_].action();
		}
	}

	void renderListItems()
	{
		int itemX = position_.x + PADDING;

		game_.setRenderScale({1, 1});

		// Highlight selection
		if (index_ >= top_ && index_ < top_ + maxVisibleItems_) {
			int itemY = itemHeight_ * (index_ - top_) + PADDING + position_.y + 1;
			int rectWidth = itemWidth_ - 4;
			int rectHeight = itemHeight_ - 2;

			// Fill the rectangle with a light gray color
			game_.fillRectangle(Recti{itemX + 2, itemY, rectWidth, rectHeight}, {240, 240, 240, 255});

			// Draw the outer rectangle with a dark gray color
			game_.drawRectangle(Recti{itemX + 2, itemY, rectWidth, rectHeight}, {40, 40, 40, 255});

			// Draw the inner rectangle with a lighter gray color
			game_.drawRectangle(Recti{itemX + 3, itemY + 1, rectWidth - 2, rectHeight - 2}, {100, 100, 100, 255});
		}

		for (size_t i = 0; i < maxVisibleItems_ && top_ + i < items_.size(); ++i) {
			Vec2i pos = {itemX + PADDING, itemHeight_ * static_cast<int>(i) + PADDING * 2 + position_.y};
			game_.drawText(Recti{pos.x, pos.y, itemWidth_, FONT_SIZE}, items_[top_ + i].name);
		}

		game_.setRenderScale({PIXEL_SIZE, PIXEL_SIZE});
	}

	void initializeMenu(Vec2i position, int menuWidth, int maxVisibleItems)
	{
		position_ = position;
		menuWidth_ = menuWidth;
		maxVisibleItems_ = maxVisibleItems;
		itemWidth_ = menuWidth_ - 2 * PADDING;
		itemHeight_ = FONT_SIZE + 2 * PADDING;
		menuHeight_ = calculateMenuHeight();
	}

	int calculateMenuHeight()
	{
		return itemHeight_ * std::min(static_cast<int>(items_.size()), maxVisibleItems_) + 2 * PADDING;
	}
};
