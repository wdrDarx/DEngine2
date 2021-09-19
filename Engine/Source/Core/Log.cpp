#pragma once
#include "Log.h"

void LogWarning(const std::string& message)
{
	std::cout << "WARNING : " << message << std::endl;
}

void LogTemp(const std::string& message)
{
	std::cout << "LOG : " << message << std::endl;
}

void LogError(const std::string& message)
{
	std::cout << "ERROR : " << message << std::endl;
}

std::string Log::string(const vec3d& in)
{
	return "X: " + std::to_string(in.x) + " Y: " + std::to_string(in.y) + " Z: " + std::to_string(in.z);
}

std::string Log::string(const vec4d& in)
{
	return "X: " + std::to_string(in.x) + " Y: " + std::to_string(in.y) + " Z: " + std::to_string(in.z) + " W: " + std::to_string(in.w);
}

std::string Log::string(const vec2d& in)
{
	return "X: " + std::to_string(in.x) + " Y: " + std::to_string(in.y);
}

std::string Log::string(const float& in)
{
	return std::to_string(in);
}
