#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.h"
#include "stb_image.h"

Image::Image(const std::string& path)
{
	LoadImage(path);
}

Image::~Image()
{
	FreeImage();
}

float* Image::LoadHDR(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	pixels = (byte*)stbi_loadf(path.c_str(), &Width, &Height, &BPP, 3);
	return (float*)pixels;
}

byte* Image::LoadImage(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	pixels = stbi_load(path.c_str(), &Width, &Height, &BPP, 4);
	return pixels;
}

byte* Image::LoadPNG(const std::string& path, const uint& channels)
{
	stbi_set_flip_vertically_on_load(1);
	pixels = stbi_load(path.c_str(), &Width, &Height, &BPP, channels);
	return pixels;
}

void Image::FreeImage()
{
	if(pixels)
		stbi_image_free(pixels);
}
