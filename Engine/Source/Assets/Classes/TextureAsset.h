#pragma once
#include "Assets/Asset.h"
#include "Rendering/Texture.h"
#include "Utils/ImageLoader.h"
#include "Serialization/Buffer.h"

class DENGINE_API TextureAsset : public Asset
{
public:
	TextureAsset() : Asset()
	{

	}

	~TextureAsset()
	{

	}

	TextureAsset(const Image& image) : m_width(image.Width), m_height(image.Height), m_Pixels(Buffer(image.pixels, image.pixels + image.Width * image.Height * 4))
	{

	}

	TextureAsset(const uint& width, const uint& height, const Buffer& pixels) : m_width(width), m_height(height), m_Pixels(pixels)
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITE(&m_width, 4);
		WRITE(&m_height, 4);
		WRITEBUFFER(m_Pixels);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READ(&m_width, 4);
		READ(&m_height, 4);
		READBUFFER(m_Pixels);
		STOPREAD();
	}

	Ref<Texture> GetTexture()
	{
		if (!m_LoadedTexture)
		{
			m_LoadedTexture = MakeRef<Texture>(m_width, m_height, m_Pixels.data());
			return m_LoadedTexture;
		}
		else
			return m_LoadedTexture;
	}

	uint m_width = 0;
	uint m_height = 0;
	Buffer m_Pixels;
	Ref<Texture> m_LoadedTexture;
};