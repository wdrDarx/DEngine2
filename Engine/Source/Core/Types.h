#pragma once
#include <cmath>
#include <algorithm>
#include "glm/glm.hpp"
#define USE_GLM_TYPES

//# define M_PI  3.14159265358979323846  /* pi */
constexpr float M_PI = (float)3.14159265358979323846; /* pi */

const float DegToRad = M_PI / 180.f;
const float RadToDeg = 180.f / M_PI;

typedef uint32_t uint;
typedef uint64_t uint64;
typedef uint8_t byte;

#ifdef USE_GLM_TYPES
using color4 = glm::vec4;
using vec3d = glm::vec3;
using vec2d = glm::vec2;
using rotator = glm::quat;

struct Transform
{
	vec3d pos;
	vec3d rot;
	vec3d scale;

	Transform() : pos({ 0,0,0 }), rot({ 0,0,0 }), scale({ 1,1,1 })
	{

	}

	Transform(const vec3d& p, const vec3d& r, const vec3d& s) : pos(p), rot(r), scale(s)
	{}

	Transform operator+(const Transform& rhs) const
	{
		return Transform(this->pos + rhs.pos, this->rot + rhs.rot, this->scale + rhs.scale);
	}

	Transform operator-(const Transform& rhs) const
	{
		return Transform(this->pos - rhs.pos, this->rot - rhs.rot, this->scale - rhs.scale);
	}

	bool operator==(const Transform& rhs) const
	{
		return this->pos == rhs.pos && this->rot == rhs.rot && this->scale == rhs.scale;
	}

	void operator=(const Transform& rhs)
	{
		this->pos = rhs.pos;
		this->rot = rhs.rot;
		this->scale = rhs.scale;
	}
};

struct Bound
{
	vec3d min;
	vec3d max;
};
#endif

#if 0
struct vec2d
{
	float x;
	float y;

	vec2d() : x(0), y(0)
	{}

	vec2d(const float& inx, const float& iny) : x(inx), y(iny)
	{}

	const bool operator==(const vec2d& rhs) const
	{
		return (rhs.x == this->x) && (rhs.y == this->y);
	}

	const bool operator>(const vec2d& rhs) const
	{
		return (this->x > rhs.x) && (this->y > rhs.y);
	}

	const bool operator!=(const vec2d& rhs)
	{
		return (rhs.x != this->x) || (rhs.y != this->y);
	}
	const vec2d operator+(const vec2d& rhs) const
	{
		return vec2d(rhs.x + this->x, rhs.y + this->y);
	}
	const vec2d operator-(const vec2d& rhs) const
	{
		return vec2d(this->x - rhs.x, this->y - rhs.y);
	}
	const vec2d operator*(const vec2d& rhs)
	{
		return vec2d(rhs.x * this->x, rhs.y * this->y);
	}
	const vec2d operator*(const float& rhs) const
	{
		return vec2d(rhs * this->x, rhs * this->y);
	}
	const vec2d operator/(const vec2d& rhs) const
	{
		return vec2d(this->x / rhs.x, this->y / rhs.y);
	}
	const vec2d operator/(const float& rhs) const
	{
		return vec2d(this->x / rhs, this->y / rhs);
	}
	void operator+=(const vec2d& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
	}
};

struct vec3d
{
	float x;
	float y;
	float z;

	vec3d() : x(0), y(0), z(0)
	{}

	vec3d(const float& inx, const float& iny, const float& inz) : x(inx), y(iny), z(inz)
	{}

	bool operator==(const vec3d& rhs) const
	{
		return (rhs.x == this->x) && (rhs.y == this->y) && (rhs.z == this->z);
	}
	bool operator!=(const vec3d& rhs)
	{
		return (rhs.x != this->x) || (rhs.y != this->y) && (rhs.z != this->z);
	}
	const vec3d operator+(const vec3d& rhs) const
	{
		return vec3d(rhs.x + this->x, rhs.y + this->y, rhs.z + this->z);
	}
	const vec3d operator-(const vec3d& rhs) const
	{
		return vec3d(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}
	vec3d operator*(const vec3d& rhs) const
	{
		return vec3d(rhs.x * this->x, rhs.y * this->y, rhs.z * this->z);
	}
	vec3d operator*(const float& rhs) const
	{
		return vec3d(rhs * this->x, rhs * this->y, rhs * this->z);
	}
	vec3d operator/(const vec3d& rhs) const
	{
		return vec3d(std::max<float>(this->x / rhs.x, 1.0f), std::max<float>(this->y / rhs.y, 1.0f), std::max<float>(this->z / rhs.z, 1.0f));
	}
};

struct color3
{
	float r;
	float g;
	float b;

	color3() : r(0), b(0), g(0)
	{

	}

	color3(const float& inr, const float& ing, const float& inb) : r(inr), g(ing), b(inb)
	{}

	bool operator==(const color3& rhs)
	{
		return (rhs.r == this->r) && (rhs.g == this->g) && (rhs.b == this->b);
	}
	bool operator!=(const color3& rhs)
	{
		return (rhs.r != this->r) && (rhs.g != this->g) && (rhs.b != this->b);
	}
	color3 operator+(const color3& rhs)
	{
		return color3(rhs.r + this->r, rhs.g + this->g, rhs.b + this->b);
	}
	color3 operator-(const color3& rhs)
	{
		return color3(this->r - rhs.r, this->g - rhs.g, this->b - rhs.b);
	}
	color3 operator*(const color3& rhs)
	{
		return color3(rhs.r * this->r, rhs.g * this->g, rhs.b * this->b);
	}
	color3 operator/(const color3& rhs)
	{
		return color3(this->r / rhs.r, this->g / rhs.g, this->b / rhs.b);
	}

	color3 operator*(const float& rhs) const
	{
		return color3(this->r / rhs, this->g / rhs, this->b / rhs);
	}
};

#endif