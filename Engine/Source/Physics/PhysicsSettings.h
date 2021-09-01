#pragma once
#include "Core/Core.h"
#include "PhysX/PxPhysicsAPI.h"

enum class CollisionDetectionType 
{ 
	Discrete,
	Continuous,
	ContinuousSpeculative 
};

enum class BroadphaseType
{
	SweepAndPrune,
	MultiBoxPrune,
	AutomaticBoxPrune
};

enum class FrictionType
{
	Patch,
	OneDirectional,
	TwoDirectional
};

enum class DebugType : int8_t
{
	DebugToFile = 0,
	LiveDebug = 1
};

struct PhysicsSettings
{
	float FixedTimestep = 1.0f / 80.0f;
	vec3d Gravity = { 0.0f, -1100.f, 0.0f };
	BroadphaseType BroadphaseAlgorithm = BroadphaseType::AutomaticBoxPrune;
	FrictionType FrictionModel = FrictionType::Patch;
	uint SolverIterations = 8;
	uint SolverVelocityIterations = 2;
	DebugType DebugType = DebugType::LiveDebug;
};