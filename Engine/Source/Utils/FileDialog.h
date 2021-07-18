#pragma once
#include "Core/Core.h"

class DENGINE_API FileDialog
{
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};

