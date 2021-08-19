#pragma once

#include "Core/Core.h"

/*
	Class to contain 2d texture data
	Texture is unlaoded on destruction
*/
struct DENGINE_API Image
{
	Image() {};

	/*
		Loads in a texture from a file path to a .png or .jpg
	*/
	Image(const std::string& path);

	~Image();

	/*
		Loads in .HDR images
	*/
	float* LoadHDR(const std::string& path);

	/*
		Loads in a texture from a file path to a .png or .jpg
	*/
	byte* LoadImage(const std::string& path);

	/*
		Loads in a texture from a file path to a .png or .jpg
		with a specified amount of channels
	*/
    byte* LoadPNG(const std::string& path, const uint& channels);

	/*
		frees the image data
	*/
	void FreeImage();


	int Width = 0;
	int Height = 0;
	int BPP = 0;
	TextureSpec m_Spec;

	byte* pixels = nullptr;
};



