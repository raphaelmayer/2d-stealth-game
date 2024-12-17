#pragma once

#include "../ui/UIElement.hpp"
#include <memory>
#include <vector>

// Stack-like structure which contains TextContainers.
// MenuStack::update renders all UIElement elements in the stack from bottom to top.
// Only handles input for the top menu, allowing background menus to be visible
// but inactive.
class MenuStack {
  public:
	void push(std::unique_ptr<UIElement> uiElement) { stack.push_back(std::move(uiElement)); }

	// Template method to automatically handle construction of unique_ptr<UIElement>.
	// Not sure if this is even desirable. Kind of hides the lifetime from the caller side.
	template <typename T, typename... Args>
	void push(Args &&...args)
	{
		static_assert(std::is_base_of<UIElement, T>::value, "T must be derived from UIElement");
		auto uiElement = std::make_unique<T>(std::forward<Args>(args)...);
		push(std::move(uiElement));
	}

	std::unique_ptr<UIElement> pop()
	{
		if (stack.empty()) {
			return nullptr;
		}
		auto topMenu = std::move(stack.back());
		stack.pop_back();
		return topMenu;
	}

	bool isEmpty() const { return stack.size() == 0; }

	void reset()
	{
		while (!stack.empty())
			stack.pop_back();
	}

	void update()
	{
		// draw all menus bottom up
		for (const auto &menu : stack)
			if (menu)
				menu->render();

		// handle inputs from menu on top
		if (!stack.empty())
			stack.back()->handleInput();
	}

  private:
	std::vector<std::unique_ptr<UIElement>> stack;
};
