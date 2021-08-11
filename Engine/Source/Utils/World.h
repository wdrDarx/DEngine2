#pragma once
#include "Core/Core.h"

/*
	2 dimentional transform
*/
struct DENGINE_API Transform2D
{
	Transform2D(const vec2d& p, const float& r, const vec2d& s) :pos(p), rot(r), scale(s)
	{

	}
	vec2d pos;
	float rot;
	vec2d scale;
};

class b2World;
struct b2RayCastOutput;
class Camera;

/*
	Util class for transformations and matrix stuff
*/
class DENGINE_API World
{
public:

	static vec3d Radians(const vec3d& rad);
	static vec3d Degrees(const vec3d& deg);

	static float TriangleArea2D(const vec2d& v1, const vec2d& v2, const vec2d& v3);
	static bool BoxvsPointCollisionCheck2D(const vec2d& v1, const vec2d& v2, const vec2d& v3, const vec2d& v4, const vec2d& worldPos);
	static bool BoxvsPointCollisionCheck2D(const Bound& Box, const vec2d& worldPos);

// 	static vec2d ScreenSpaceToWorldSpace(Camera* camera, const vec2d& in);
// 	static vec2d WorldSpaceToScreenSpace(Camera* camera, const vec2d& in);

	static vec2d Rotate2D(const vec2d& in, const float& Angle = 0.f);
	static vec3d Rotate3D(const vec3d& in, const vec3d& rot = {0,0,0});

	static float  DotProduct2D(const vec2d& v1, const vec2d& v2);
	static vec2d CrossProduct2D(const vec2d& in1);
	static float LookAt2D(const vec2d& start, const vec2d& end);
	static float Length2D(const vec2d& in);
	static vec2d Normalize2D(const vec2d& in);

	static Transform Make3D(const vec2d& pos, const float& rot, const vec2d& scale);
	static Transform2D Make2D(const Transform& trans);

	static glm::mat4 MakeMatrix(const Transform& trans);
	static Transform MakeTransform(const glm::mat4& mat);

	static Transform OffsetChildByParent(const Transform& parent, const Transform& child);
	static Transform SubtractTransform(const Transform& A, const Transform& B);

	static vec3d GetForwardVector(const vec3d& Rot);
	static vec3d GetRightVector(const vec3d& Rot);
	static vec3d GetUpVector(const vec3d& Rot);

	static glm::quat MatToQuat(const glm::mat4& mat);
};

