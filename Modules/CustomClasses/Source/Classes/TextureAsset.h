#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES TextureAsset : public Asset
{
	public:
		TextureAsset() : Asset()
		{

		}

		~TextureAsset()
		{
			LogWarning("ok");
		}

		TextureAsset(const Image& image) : m_width(image.Width), m_height(image.Height), m_Pixels(std::vector<byte>(image.pixels, image.pixels + image.Width * image.Height * 4))
		{

		}

		TextureAsset(const uint& width, const uint& height, const std::vector<unsigned char>& pixels) : m_width(width), m_height(height), m_Pixels(pixels)
		{

		}

		uint Serialize(Buffer& buffer) const override
		{
			STARTWRITE(buffer, Asset::Serialize(buffer))
			WRITE(&m_width, 4);
			WRITE(&m_height, 4);
			WRITEVEC(m_Pixels, 1);
			STOPWRITE();
		}

		uint Deserialize(const Buffer& buffer) override
		{
			STARTREAD(buffer, Asset::Deserialize(buffer))
			READ(&m_width, 4);
			READ(&m_height, 4);
			READVEC(m_Pixels, 1);
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
		std::vector<byte> m_Pixels;
		Ref<Texture> m_LoadedTexture;
};