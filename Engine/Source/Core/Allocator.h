#pragma once
#include "Core/Core.h"

struct DENGINE_API Allocator
{
	public:
		void* Allocate(size_t size);
};