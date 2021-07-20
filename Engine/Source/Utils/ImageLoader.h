#pragma once
//#define STB_IMAGE_IMPLEMENTATION
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
    unsigned char* LoadImage(const std::string& path);
    unsigned char* LoadPNG(const std::string& path, const uint& channels);
	void FreeImage();


	int Width = 0;
	int Height = 0;
	int BPP = 0;

	unsigned char* pixels = nullptr;
};



