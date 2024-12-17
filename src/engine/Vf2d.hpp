#pragma once

#include "../constants.hpp"
#include "Vec2d.hpp"
#include <cassert>
#include <cmath>

// Currently not used anywhere.
struct Vf2d {
	float x;
	float y;

	bool operator==(const Vf2d &v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vf2d &v) const { return !operator==(v); }

	Vf2d operator+(float n) const { return {x + n, y + n}; }
	Vf2d operator+(const Vf2d &v) const { return {x + v.x, y + v.y}; }

	Vf2d operator-(float n) const { return {x - n, y - n}; }
	Vf2d operator-(const Vf2d &v) const { return {x - v.x, y - v.y}; }

	Vf2d operator*(const Vf2d &v) const { return {x * v.x, y * v.y}; }
	Vf2d operator*(float n) const { return {x * n, y * n}; }

	Vf2d operator/(const Vf2d &v) const
	{
		assert(v.x != 0.0f && v.y != 0.0f);
		return {x / v.x, y / v.y};
	}
	Vf2d operator/(float n) const
	{
		assert(n != 0.0f);
		return {x / n, y / n};
	}

	Vf2d &operator+=(const Vf2d &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vf2d &operator+=(float n)
	{
		x += n;
		y += n;
		return *this;
	}

	Vf2d &operator-=(const Vf2d &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vf2d &operator-=(float n)
	{
		x -= n;
		y -= n;
		return *this;
	}

	Vf2d &operator*=(const Vf2d &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vf2d &operator*=(float n)
	{
		x *= n;
		y *= n;
		return *this;
	}

	Vf2d &operator/=(const Vf2d &v)
	{
		assert(v.x != 0.0f && v.y != 0.0f);
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vf2d &operator/=(float n)
	{
		assert(n != 0.0f);
		x /= n;
		y /= n;
		return *this;
	}

	// Convert to 1D (assuming a certain width for the 2D space)
	float to1d(float width) const { return y * width + x; }

	// Sign function
	Vf2d sign() const { return {std::copysign(1.0f, x), std::copysign(1.0f, y)}; }

	// Convert vector from pixel domain to tile domain, e.g.: (32.0f, 32.0f).toTileSize() == (2.0f, 2.0f)
	// Also rounds the result.
	Vf2d toTileSize() const { return {std::round(x / TILE_SIZE), std::round(y / TILE_SIZE)}; }

	// Normalize the vector
	Vf2d norm() const
	{
		float magnitude = std::sqrt(x * x + y * y);
		assert(magnitude != 0.0f);
		return {x / magnitude, y / magnitude};
	}

	// Convert to Vec2d (integer version of the vector)
	Vec2d toVec2d() const { return Vec2d(static_cast<int>(x), static_cast<int>(y)); }

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};
