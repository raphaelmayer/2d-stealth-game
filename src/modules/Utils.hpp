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

Vec2i to2d(int n, int width)
{
	return Vec2i{n % width, n / width};
}

int to1d(const Vec2i &vec, int width)
{
	return vec.y * width + vec.x;
}

void print(const Vec2f &vec)
{
	std::cout << vec.x << ", " << vec.y << '\n';
}

void print(const Vec2i &vec)
{
	std::cout << vec.x << ", " << vec.y << '\n';
}

Rectf vectorsToRectangle(const Vec2f &vec1, const Vec2f &vec2)
{
	Rectf rect{};

	// Calculate the top-left corner (x, y)
	rect.x = std::min(vec1.x, vec2.x);
	rect.y = std::min(vec1.y, vec2.y);

	// Calculate the width and height
	rect.w = std::abs(vec1.x - vec2.x);
	rect.h = std::abs(vec1.y - vec2.y);

	return rect;
}

Rotation vec2fToRotation(const Vec2f &vec)
{
	// Because we used "toTarget.norm()" it could have any angle,
	// but we only handle 4 directions, so pick the largest axis.
	if (std::fabs(vec.x) > std::fabs(vec.y)) {
		// Horizontal movement
		if (vec.x > 0.f) {
			return Rotation::EAST;
		} else {
			return Rotation::WEST;
		}
	} else {
		// Vertical movement
		if (vec.y > 0.f) {
			return Rotation::SOUTH;
		} else {
			return Rotation::NORTH;
		}
	}
}

Vec2f rotationToVec2f(Rotation rotation)
{

	switch (rotation) {
	case NORTH:
		return {0, -1};
		break;
	case EAST:
		return {1, 0};
		break;
	case SOUTH:
		return {0, 1};
		break;
	case WEST:
		return {-1, 0};
		break;
	}
	return {0, 0};
}

} // namespace Utils