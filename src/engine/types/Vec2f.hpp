#pragma once

#include "../constants.hpp"
#include "Vec2i.hpp"
#include <cassert>
#include <cmath>

// Currently not used anywhere.
struct Vec2f {
	float x;
	float y;

	bool operator==(const Vec2f &v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vec2f &v) const { return !operator==(v); }

	Vec2f operator+(float n) const { return {x + n, y + n}; }
	Vec2f operator+(const Vec2f &v) const { return {x + v.x, y + v.y}; }

	Vec2f operator-(float n) const { return {x - n, y - n}; }
	Vec2f operator-(const Vec2f &v) const { return {x - v.x, y - v.y}; }

	Vec2f operator*(const Vec2f &v) const { return {x * v.x, y * v.y}; }
	Vec2f operator*(float n) const { return {x * n, y * n}; }

	Vec2f operator/(const Vec2f &v) const
	{
		assert(v.x != 0.0f && v.y != 0.0f);
		return {x / v.x, y / v.y};
	}
	Vec2f operator/(float n) const
	{
		assert(n != 0.0f);
		return {x / n, y / n};
	}

	Vec2f &operator+=(const Vec2f &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec2f &operator+=(float n)
	{
		x += n;
		y += n;
		return *this;
	}

	Vec2f &operator-=(const Vec2f &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vec2f &operator-=(float n)
	{
		x -= n;
		y -= n;
		return *this;
	}

	Vec2f &operator*=(const Vec2f &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vec2f &operator*=(float n)
	{
		x *= n;
		y *= n;
		return *this;
	}

	Vec2f &operator/=(const Vec2f &v)
	{
		assert(v.x != 0.0f && v.y != 0.0f);
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vec2f &operator/=(float n)
	{
		assert(n != 0.0f);
		x /= n;
		y /= n;
		return *this;
	}

	// Convert to 1D (assuming a certain width for the 2D space)
	float to1d(float width) const { return y * width + x; }

	// Sign function
	Vec2f sign() const { return {std::copysign(1.0f, x), std::copysign(1.0f, y)}; }

	// Convert vector from pixel domain to tile domain, e.g.: (32.0f, 32.0f).toTileSize() == (2.0f, 2.0f)
	// Also rounds the result.
	Vec2f toTileSize() const { return {std::round(x / TILE_SIZE), std::round(y / TILE_SIZE)}; }

	// Normalize the vector
	Vec2f norm() const
	{
		float magnitude = std::sqrt(x * x + y * y);
		assert(magnitude != 0.0f);
		return {x / magnitude, y / magnitude};
	}

	// Convert to Vec2d (integer version of the vector)
	Vec2i toVec2d() const { return Vec2i(static_cast<int>(x), static_cast<int>(y)); }

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};
