#pragma once

#include "../constants.hpp"
#include "../engine/types.hpp"
#include <cmath>
#include <iostream>

namespace Utils {

Vec2i toInt(const Vec2f &vec)
{
	return Vec2i{static_cast<int>(vec.x), static_cast<int>(vec.y)};
}

Vec2f toFloat(const Vec2i &vec)
{
	return Vec2f{static_cast<float>(vec.x), static_cast<float>(vec.y)};
}

Vec2f round(const Vec2f &vec)
{
	return {std::round(vec.x), std::round(vec.y)};
}

Recti toInt(const Rectf &rect)
{
	return Recti{static_cast<int>(rect.x), static_cast<int>(rect.y), static_cast<int>(rect.w),
	             static_cast<int>(rect.h)};
}

Rectf toFloat(const Recti &rect)
{
	return Rectf{static_cast<float>(rect.x), static_cast<float>(rect.y), static_cast<float>(rect.w),
	             static_cast<float>(rect.h)};
}

Vec2i toTileSize(const Vec2i &vec, const int tileSize = TILE_SIZE)
{
	return Vec2i{vec.x / tileSize, vec.y / tileSize};
}

Vec2i toTileSize(const Vec2f &vec, const int tileSize = TILE_SIZE)
{
	return Vec2i{static_cast<int>(vec.x) / tileSize, static_cast<int>(vec.y) / tileSize};
}

Vec2i toGrid(const Vec2i &vec, const int tileSize = TILE_SIZE)
{
	return toTileSize(vec) * TILE_SIZE;
}

Vec2i toGrid(const Vec2f &vec, const int tileSize = TILE_SIZE)
{
	return toTileSize(vec) * TILE_SIZE;
}

Vec2f discreteDirection4_signBased(const Vec2f &vec)
{
	float absX = std::fabs(vec.x);
	float absY = std::fabs(vec.y);

	// If the vector is basically zero:
	if (absX < 1e-6f && absY < 1e-6f) {
		return Vec2f(0.f, 0.f);
	}

	// If |x| > |y|, we choose horizontal axis; otherwise vertical.
	if (absX >= absY) {
		return Vec2f((vec.x >= 0.f) ? 1.f : -1.f, 0.f);
	} else {
		return Vec2f(0.f, (vec.y >= 0.f) ? 1.f : -1.f);
	}
}


void print(const Vec2f &vec)
{
	std::cout << vec.x << ", " << vec.y << '\n';
}

void print(const Vec2i &vec)
{
	std::cout << vec.x << ", " << vec.y << '\n';
}

} // namespace Utils