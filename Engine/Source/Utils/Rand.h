#pragma once
#include <random>
#include "Core/Core.h"

#define EXPORT_FILE //export this 
/*
	static class that provides random value functions (MAKE SURE TO CALL THE RAND FUNCTIONS FROM THE ENGINE MODULE FILES BECAUSE THE RANDOM ENGINE IS A STATIC VAR)
*/
class Rand
{
public:
	Rand();
	static Rand& Get();

	static float Float();
	static uint Int();
	static uint64 Int64();

private:
	static Rand m_Instance;

	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

#pragma data_seg("SHARED")  // Begin the shared data segment.
	DENGINE_API static bool s_IsInit;
#pragma data_seg()          // End the shared data segment and default back to 
};
#pragma comment(linker, "/section:SHARED,RWS")
#undef EXPORT_FILE