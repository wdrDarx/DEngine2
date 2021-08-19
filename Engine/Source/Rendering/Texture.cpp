#pragma once
#include "Texture.h"
#include "Core/Core.h"
#include "Utils/ImageLoader.h"

#define ANISOTROPIC_FILTERING 1

void Texture::Construct(const TextureSpec& spec, const uint& width, const uint& height, byte* pixeldata)
{
	m_Width = width;
	m_Height = height;

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(spec.Type == TextureType::RGBA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);
	else if(spec.Type == TextureType::HDR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, (float*)pixeldata);

	glGenerateMipmap(GL_TEXTURE_2D);

	if (ANISOTROPIC_FILTERING)
	{
		GLfloat MaxAnisotripy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MaxAnisotripy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, MaxAnisotripy);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const TextureSpec& spec)
{
	uint32_t data = 0xffffffff;
	Construct(spec, 1, 1, (byte*)&data);
}

Texture::Texture(const TextureSpec& spec, const uint& width, const uint& height, byte* pixeldata)
{
	Construct(spec, width, height, pixeldata);
}

Texture::Texture(const Image& image)
{
	Construct(image.m_Spec, image.Width, image.Height, image.pixels);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(uint slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

