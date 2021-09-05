#pragma once
#include "Assets/Asset.h"
#include "Rendering/Texture.h"
#include "Utils/ImageLoader.h"
#include "Serialization/Buffer.h"

class TextureAsset : public Asset
{
public:
	TextureAsset() : Asset()
	{
		
	}

	~TextureAsset()
	{

	}

	TextureAsset(const Image& image) : m_width(image.Width), m_height(image.Height), m_Spec(image.m_Spec)
	{
		if(m_Spec.Type == TextureType::RGBA)
			m_Pixels = Buffer(image.pixels, image.pixels + image.Width * image.Height * 4);
		if (m_Spec.Type == TextureType::HDR)
			m_Pixels = Buffer(image.pixels, image.pixels + image.Width * image.Height * 3 * 4); //3 channels, 4 bytes per channel
	}

	TextureAsset(const uint& width, const uint& height, const Buffer& pixels) : m_width(width), m_height(height), m_Pixels(pixels)
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITE(&m_width, 4);
		WRITE(&m_height, 4);
		WRITE(&m_Spec, sizeof(TextureSpec));
		WRITEBUFFER(m_Pixels);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READ(&m_width, 4);
		READ(&m_height, 4);
		READ(&m_Spec, sizeof(TextureSpec));
		READBUFFER(m_Pixels);
		STOPREAD();
	}

	Ref<Texture> GetTexture()
	{
		if (!m_LoadedTexture)
		{
			m_LoadedTexture = MakeRef<Texture>(m_Spec, m_width, m_height, m_Pixels.data());

			if (!GetAssetIcon())
			{
				m_AssetIcon = MakeRef<AssetIcon>();
				m_AssetIcon->m_Texture = m_LoadedTexture;
			}

			return m_LoadedTexture;
		}
		else
			return m_LoadedTexture;
	}

	uint m_width = 1;
	uint m_height = 1;
	TextureSpec m_Spec;
	Buffer m_Pixels;
	Ref<Texture> m_LoadedTexture;
};