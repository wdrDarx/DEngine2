#pragma once
#include <random>
#include "Core/Core.h"
/*
	static class that provides random value functions (MAKE SURE TO CALL THE RAND FUNCTIONS FROM THE ENGINE MODULE FILES BECAUSE THE RANDOM ENGINE IS A STATIC VAR)
*/
class DENGINE_API Rand
{
public:
	static void Init();

	static float Float();
	static uint Int();
	static uint64_t Int64();

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	static bool s_IsInit;
};