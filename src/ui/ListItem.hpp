#pragma once

#include <string>
#include <functional>

struct ListItem {
	std::string name;
	std::function<void()> action;
};