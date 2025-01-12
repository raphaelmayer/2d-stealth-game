#pragma once

#include "../constants.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2i.hpp"
#include "../modules/MenuStack.hpp"
#include "UIElement.hpp"
#include <string>

// A UI component for displaying and managing text-based dialogue or messages.
// Capable of handling single or multi-line text, this class provides functionality for user interaction
// through advancing text with key presses and closing the dialogue.
class TextDialog final : public UIElement {
  public:
	TextDialog(Engine &game, MenuStack &menuStack, std::string text = "NULL")
	    : UIElement(game), game_(game), menuStack_(menuStack), text_({text})
	{
	}
	TextDialog(Engine &game, MenuStack &menuStack, std::vector<std::string> text)
	    : UIElement(game), game_(game), menuStack_(menuStack), text_(text)
	{
	}

	void handleInput()
	{
		if (game_.getKeyState(SDL_SCANCODE_ESCAPE).pressed)
			close();
		if (game_.getKeyState(SDL_SCANCODE_RETURN).pressed)
			advance();
	}

	void render()
	{
		renderContainer(Vec2i{x, y}, menuWidth_, menuHeight_);
		renderText();
	}

	void setText(std::string text) { text_ = {text}; }
	void setText(std::vector<std::string> text) { text_ = text; }

	void advance()
	{
		if (++currentLine >= text_.size())
			close();
	}

	void close() { menuStack_.pop(); }

  private:
	void renderText()
	{
		game_.setRenderScale({1, 1});

		if (currentLine < text_.size()) {
			game_.drawText(Recti{x + PADDING * 2, y + PADDING * 2, menuWidth_ - 2 * PADDING,
			                FONT_SIZE},
			               text_[currentLine]);
		}

		game_.setRenderScale({PIXEL_SIZE, PIXEL_SIZE});
	}

	Engine &game_;
	MenuStack &menuStack_;
	std::vector<std::string> text_;
	int currentLine = 0;

	static constexpr int margin = 10;
	static constexpr int menuWidth_ = WINDOW_WIDTH * PIXEL_SIZE - 2 * margin;
	static constexpr int menuHeight_ = 90;
	static constexpr int x = margin;
	static constexpr int y = WINDOW_HEIGHT * PIXEL_SIZE - menuHeight_ - margin;
};
