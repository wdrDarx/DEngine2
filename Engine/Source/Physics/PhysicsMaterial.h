#pragma once
#include "Core/Core.h"

struct DENGINE_API PhysicsMaterial
{
	float StaticFriction = 0.5f;
	float DynamicFriction = 0.5f;
	float Bounciness = 0.1f;
};