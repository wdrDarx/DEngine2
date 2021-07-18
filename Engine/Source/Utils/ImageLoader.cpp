#include "ImageLoader.h"
#include "stb_image.h"

Image::Image(const std::string& path)
{
	this->LoadImage(path);
}

Image::~Image()
{
	this->FreeImage();
}

float* Image::LoadHDR(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	pixels = (unsigned char*)stbi_loadf(path.c_str(), &Width, &Height, &BPP, 3);
	return (float*)pixels;
}

unsigned char* Image::LoadImage(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	this->pixels = stbi_load(path.c_str(), &this->Width, &this->Height, &this->BPP, 4);
	return pixels;
}

unsigned char* Image::LoadPNG(const std::string& path, const uint& channels)
{
	stbi_set_flip_vertically_on_load(1);
	this->pixels = stbi_load(path.c_str(), &this->Width, &this->Height, &this->BPP, channels);
	return pixels;
}

void Image::FreeImage()
{
	if(this->pixels)
		stbi_image_free(this->pixels);
}
