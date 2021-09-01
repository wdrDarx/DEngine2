#include "Rand.h"

Rand::Rand()
{
	s_RandomEngine.seed(std::random_device()());
	s_IsInit = true;
}

Rand& Rand::Get()
{
	return m_Instance;
}

float Rand::Float()
{
	return (float)Get().s_Distribution(Get().s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
}

uint Rand::Int()
{
	return (uint)Get().s_Distribution(Get().s_RandomEngine);
}

uint64 Rand::Int64()
{
	return (uint64_t)Get().Int() * (uint64_t)Get().Int();
}

bool Rand::s_IsInit = false;

Rand Rand::m_Instance;
std::mt19937 Rand::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Rand::s_Distribution;

