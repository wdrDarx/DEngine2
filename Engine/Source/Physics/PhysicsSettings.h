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
	AutomaticBoxPrune,
	GPU
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
	float FixedTimestep = 1.0f / 60.0f;
	vec3d Gravity = { 0.0f, -981.f, 0.0f };
	BroadphaseType BroadphaseAlgorithm = BroadphaseType::GPU;
	FrictionType FrictionModel = FrictionType::Patch;
	uint SolverIterations = 6;
	uint SolverVelocityIterations = 1;
	DebugType DebugType = DebugType::LiveDebug;
};