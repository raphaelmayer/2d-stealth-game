#pragma once

#include "../constants.hpp"
#include "../engine/types.hpp"

namespace Utils {

Vec2i toInt(const Vec2f &vec)
{
	return Vec2i{static_cast<int>(vec.x), static_cast<int>(vec.y)};
}

Vec2f toFloat(const Vec2i &vec)
{
	return Vec2f{static_cast<float>(vec.x), static_cast<float>(vec.y)};
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

} // namespace Utils