#pragma once
#include "Core/Types.h"
#include <string>
#include <iostream>

#define STRING(x) std::to_string(x)

void LogWarning(const std::string& message);
void LogTemp(const std::string& message);
void LogError(const std::string& message);

class Log
{
public:
	static std::string string(const vec3d& in);
	static std::string string(const vec4d& in);
	static std::string string(const vec2d& in);
	static std::string string(const float& in);
};