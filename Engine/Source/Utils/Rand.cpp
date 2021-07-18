#include "Rand.h"

void Rand::Init()
{
  s_RandomEngine.seed(std::random_device()());
}

float Rand::Float()
{
	return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
}

uint Rand::Int()
{
	return (uint)s_Distribution(s_RandomEngine);
}

uint64_t Rand::Int64()
{
	return (uint64_t)Rand::Int() * (uint64_t)Rand::Int();
}

std::mt19937 Rand::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Rand::s_Distribution;

