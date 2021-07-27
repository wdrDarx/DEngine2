#pragma once

#include "Core/Core.h"

/*
	Class to contain 2d texture data
*/
struct DENGINE_API Image
{
	Image() {};
	Image(const std::string& path);
	~Image();

	float* LoadHDR(const std::string& path);
	byte* LoadImage(const std::string& path);
    byte* LoadPNG(const std::string& path, const uint& channels);
	void FreeImage();


	int Width = 0;
	int Height = 0;
	int BPP = 0;

	byte* pixels = nullptr;
};



