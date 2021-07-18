#pragma once
#if 0
#include "World.h"
#include "Rendering/Core/Renderer.h"
#include "Rendering/Core/Window.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Core/Core.h"
#include "Rendering/2D/Camera.h"
#include "GLFW/glfw3.h"
#include "Box2D/box2d.h"

vec3d World::Degrees(const vec3d& rad)
{
	glm::vec3 rads = *(glm::vec3*)&rad;
	return *(vec3d*)&glm::vec3(glm::degrees(rads.x), glm::degrees(rads.y), glm::degrees(rads.z));
}


float World::TriangleArea2D(const vec2d& v1, const vec2d& v2, const vec2d& v3)
{
	float a = World::Length2D(v2 - v1);
	float b = World::Length2D(v3 - v2);
	float c = World::Length2D(v3 - v1);
	float semiperimiter = (a+b+c) / 2.0f;
	float out = sqrt(semiperimiter * (semiperimiter - a) * (semiperimiter - b) * (semiperimiter - c));
	return out;
}

bool World::BoxvsPointCollisionCheck2D(const vec2d& v1, const vec2d& v2, const vec2d& v3, const vec2d& v4, const vec2d& worldPos)
{
	return (DotProduct2D(worldPos - v2, v1 - v2) > 0.f &&
		DotProduct2D(worldPos - v2, v3 - v2) > 0.f &&
		DotProduct2D(worldPos - v1, v2 - v1) > 0.f &&
		DotProduct2D(worldPos - v1, v4 - v1) > 0.f &&
		DotProduct2D(worldPos - v4, v3 - v4) > 0.f &&
		DotProduct2D(worldPos - v4, v1 - v4) > 0.f &&
		DotProduct2D(worldPos - v3, v4 - v3) > 0.f &&
		DotProduct2D(worldPos - v3, v2 - v3) > 0.f
		);
}

bool World::BoxvsPointCollisionCheck2D(const Bound& Box, const vec2d& worldPos)
{
	return BoxvsPointCollisionCheck2D({Box.min.x, Box.min.y}, {Box.min.x, Box.max.y}, {Box.max.x, Box.max.y}, {Box.max.x, Box.min.y}, worldPos);
}


vec3d World::Radians(const vec3d& deg)
{
	glm::vec3 degs = *(glm::vec3*) & deg;
	return *(vec3d*)&glm::vec3(glm::radians(degs.x), glm::radians(degs.y), glm::radians(degs.z));
}

vec2d World::ScreenSpaceToWorldSpace(Camera* camera, const vec2d& in)
{
	float width = (float)camera->GetTargetWindow()->GetRenderer()->m_FrameBuffer->GetSpec().Width;
	float height = (float)camera->GetTargetWindow()->GetRenderer()->m_FrameBuffer->GetSpec().Height;
	vec2d out2 = in;
	out2.x = (2.0f * ((float)(in.x - 0) / (width - 0))) - 1.0f;
	out2.y = (2.0f * ((float)(in.y - 0) / (height - 0))) -1.0f;
 	glm::mat4 mat = camera->GetProjectionMatrix();
 	glm::vec4 v = glm::vec4(out2.x, out2.y, 1.f,1.f) * glm::inverse(mat);
	out2.x = v.x;
	out2.y = v.y;
	out2.x += camera->GetPostition().x;
	out2.y += camera->GetPostition().y;
	return out2;
}

vec2d World::WorldSpaceToScreenSpace(Camera* camera, const vec2d& in)
{
	uint width = camera->GetTargetWindow()->GetRenderer()->m_FrameBuffer->GetSpec().Width;
	uint height = camera->GetTargetWindow()->GetRenderer()->m_FrameBuffer->GetSpec().Height;

	glm::mat4 viewin = camera->GetViewMatrix();
	glm::mat4 projin = camera->GetProjectionMatrix();
	glm::mat4 both = viewin * projin;

	glm::vec4 out4 = glm::vec4(in.x, in.y, 0, 0) * both;
	vec2d out2 = vec2d(out4.x, out4.y);
	return out2;
}

vec2d World::Rotate2D(const vec2d& in, const float& Angle)
{
	vec2d out;
	out.x = in.x * (float)cos(Angle * DegToRad) - in.y * (float)sin(Angle * DegToRad);
	out.y = in.x * (float)sin(Angle * DegToRad) + in.y * (float)cos(Angle * DegToRad);
	return out;
}

vec3d World::Rotate3D(const vec3d& in, const vec3d& rot /*= {0,0,0}*/)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), glm::vec3(0,0,1)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), glm::vec3(1, 0, 0)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), glm::vec3(0, 1, 0));

	glm::vec4 v4  = rotation * glm::vec4(in.x, in.y, in.z, 1.0f);

	return vec3d(v4.x, v4.y, v4.z);
}

float World::DotProduct2D(const vec2d& v1, const vec2d& v2)
{
	vec2d norm1 = World::Normalize2D(v1);
	vec2d norm2 = World::Normalize2D(v2);

	return (norm1.x * norm2.x + norm1.y * norm2.y) / 2.0f;
}

vec2d World::CrossProduct2D(const vec2d& in1)
{
	return vec2d(in1.y, -in1.x);
}

float World::LookAt2D(const vec2d& start, const vec2d& end)
{
	return (float)(atan2(end.y - start.y, end.x - start.x) * (180.f / M_PI));
}

float World::Length2D(const vec2d& in)
{
	return sqrt(in.x * in.x + in.y * in.y);
}

vec2d World::Normalize2D(const vec2d& in)
{
	return in / Length2D(in);
}

Transform  World::Make3D(const vec2d& pos, const float& rot, const vec2d& scale)
{
	return Transform({pos.x, pos.y, 0}, {0,0, rot}, {scale.x, scale.y, 1.f});
}

Transform2D World::Make2D(const Transform& trans)
{
	return Transform2D({trans.pos.x, trans.pos.y}, trans.rot.z, {trans.scale.x, trans.scale.y});
}

glm::mat4 World::MakeMatrix(const Transform& trans)
{
	const Transform& copy = trans;

// 	if (copy.pos.x == 0)
// 		copy.pos.x = 0.00000001f;
// 	if (copy.pos.y == 0)
// 		copy.pos.y = 0.00000001f;
// 	if (copy.pos.z == 0)
// 		copy.pos.z = -0.00000001f;
// 
// 	if(copy.scale.x == 0)
// 		copy.scale.x = 0.00000001f;
// 	if (copy.scale.y == 0)
// 		copy.scale.y = 0.00000001f;
// 	if (copy.scale.z == 0)
// 		copy.scale.z = 0.00000001f;

	glm::vec3 rot = glm::vec3(copy.rot.x * DegToRad, copy.rot.y * DegToRad, copy.rot.z * DegToRad);
	glm::mat4 rotation = glm::toMat4(glm::quat(rot));
	
	glm::mat4 transform =
		glm::translate(glm::mat4(1.0f), glm::vec3(copy.pos.x, copy.pos.y, copy.pos.z)) *
		rotation *
		glm::scale(glm::mat4(1.0f), glm::vec3(copy.scale.x, copy.scale.y, copy.scale.z)) *
		glm::mat4(1.0f);

	return transform;
}

Transform World::MakeTransform(const glm::mat4& mat)
{
	Transform out;
	// From glm::decompose in matrix_decompose.inl

	using namespace glm;
	using T = float;

	vec3 translation;
	vec3 scale;
	vec3 rotation;

	mat4 LocalMatrix(mat);

	// Normalize the matrix.
	if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
		return Transform();

	// First, isolate perspective.  This is the messiest.
	if (
		epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
		epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
		epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
	{
		// Clear the perspective partition
		LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
		LocalMatrix[3][3] = static_cast<T>(1);
	}

	// Next take care of translation (easy).
	translation = vec3(LocalMatrix[3]);
	LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

	vec3 Row[3], Pdum3;

	// Now get scale and shear.
	for (length_t i = 0; i < 3; ++i)
		for (length_t j = 0; j < 3; ++j)
			Row[i][j] = LocalMatrix[i][j];

	// Compute X scale factor and normalize first row.
	scale.x = length(Row[0]);
	Row[0] = detail::scale(Row[0], static_cast<T>(1));
	scale.y = length(Row[1]);
	Row[1] = detail::scale(Row[1], static_cast<T>(1));
	scale.z = length(Row[2]);
	Row[2] = detail::scale(Row[2], static_cast<T>(1));

	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
#if 0
	Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (dot(Row[0], Pdum3) < 0)
	{
		for (length_t i = 0; i < 3; i++)
		{
			scale[i] *= static_cast<T>(-1);
			Row[i] *= static_cast<T>(-1);
		}
	}
#endif

	rotation.y = asin(-Row[0][2]);
	if (cos(rotation.y) != 0) {
		rotation.x = atan2(Row[1][2], Row[2][2]);
		rotation.z = atan2(Row[0][1], Row[0][0]);
	}
	else {
		rotation.x = atan2(-Row[2][0], Row[1][1]);
		rotation.z = 0;
	}

	out.pos = vec3d(translation.x, translation.y, translation.z);
	out.rot = Degrees(vec3d(rotation.x,rotation.y,rotation.z));
	out.scale = vec3d(scale.x, scale.y, scale.z);

	return out;
}

Transform World::OffsetChildByParent(const Transform& parent, const Transform& child)
{
	Transform out;
	vec3d scaledOffeset = child.pos * parent.scale;
	vec3d rotatedOffset = Rotate3D(scaledOffeset, parent.rot);
	out.pos = parent.pos + rotatedOffset;
	out.scale = parent.scale * child.scale;
	out.rot = ( parent.rot - child.rot) * 1.0f;

	return out;
}

Transform World::SubtractTransform(const Transform& A, const Transform& B)
{
	Transform out;
// 	vec3d scaledOffset = (A.pos - B.pos); /// A.scale;
// 	//scaledOffset = A.pos * B.scale;
// 	vec3d rotatedOffset = Rotate3D(scaledOffset, A.rot);
// 	out.pos = A.pos - rotatedOffset;
// 	out.scale = B.scale / A.scale;
// 	out.rot = A.rot - B.rot;
	out.pos = A.pos - B.pos;
	out.rot = A.rot - B.rot;
	out.scale = A.scale / B.scale;
	

	return out;
}

vec3d World::GetForwardVector(const vec3d& Rot)
{
	glm::quat q = glm::quat(glm::vec3(Rot.x * DegToRad, Rot.y * DegToRad, Rot.z * DegToRad));
	glm::vec3 f = glm::rotate(q, glm::vec3(0.0, 0.0, -1.0));

	return *(vec3d*)&f;
}

vec3d World::GetRightVector(const vec3d& Rot)
{
	glm::quat q = glm::quat(glm::vec3(Rot.x * DegToRad, Rot.y * DegToRad, Rot.z * DegToRad));
	glm::vec3 f = glm::rotate(q, glm::vec3(1.f, 0.0, 0.f));

	return *(vec3d*)&f;
}

vec3d World::GetUpVector(const vec3d& Rot)
{
	glm::quat q = glm::quat(glm::vec3(Rot.x * DegToRad, Rot.y * DegToRad, Rot.z * DegToRad));
	glm::vec3 f = glm::rotate(q, glm::vec3(0.f, 1.0f, 0.f));

	return *(vec3d*)&f;
}
#endif
