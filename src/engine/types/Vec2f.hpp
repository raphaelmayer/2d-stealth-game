#pragma once

#include "../../constants.hpp"
#include <cassert>
#include <cmath>

struct Vec2f {
	float x;
	float y;

	bool operator==(const Vec2f &v) const
	{
		constexpr float epsilon = 1e-2f; // Tolerance value for comparison
		return (std::abs(x - v.x) < epsilon) && (std::abs(y - v.y) < epsilon);
	}
	bool operator!=(const Vec2f &v) const
	{
		return !operator==(v);
	}

	Vec2f operator+(float n) const
	{
		return {x + n, y + n};
	}
	Vec2f operator+(const Vec2f &v) const
	{
		return {x + v.x, y + v.y};
	}

	Vec2f operator-(float n) const
	{
		return {x - n, y - n};
	}
	Vec2f operator-(const Vec2f &v) const
	{
		return {x - v.x, y - v.y};
	}

	Vec2f operator*(float n) const
	{
		return {x * n, y * n};
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

	Vec2f &operator*=(float n)
	{
		x *= n;
		y *= n;
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
	float to1d(float width) const
	{
		return y * width + x;
	}

	// Sign function
	Vec2f sign() const
	{
		return {std::copysign(1.0f, x), std::copysign(1.0f, y)};
	}

	// Normalize the vector
	Vec2f norm() const
	{
		float magnitude = std::sqrt(x * x + y * y);
		if (magnitude == 0.0f) {
			return {0.0f, 0.0f}; // Avoid division by zero
		}
		return {x / magnitude, y / magnitude};
	}

	float lengthSquared() const
	{
		return x * x + y * y;
	}

	float length() const
	{
		return std::sqrt(lengthSquared());
	}

	float dot(const Vec2f &v) const
	{
		return x * v.x + y * v.y;
	}

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};
