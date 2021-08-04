#pragma once
#include "Texture.h"
#include "Core/Core.h"
#include "Utils/ImageLoader.h"

#define ANISOTROPIC_FILTERING 1

void Texture::Construct(const uint& width, const uint& height, byte* pixeldata)
{
	m_Width = width;
	m_Height = height;

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixeldata);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (ANISOTROPIC_FILTERING)
	{
		GLfloat MaxAnisotripy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &MaxAnisotripy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, MaxAnisotripy);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string& path)
{
	Image tex(path);
	Construct(tex.Width, tex.Height, tex.pixels);
}

Texture::Texture()
{
	uint32_t data = 0xffffffff;
	Construct(1, 1, (byte*)&data);
}

Texture::Texture(const uint& width, const uint& height, byte* pixeldata)
{
	Construct(width, height, pixeldata);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_RendererID);
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

