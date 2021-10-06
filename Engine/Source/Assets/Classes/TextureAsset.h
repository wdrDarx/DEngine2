#pragma once
#include "Assets/Asset.h"
#include "Rendering/Texture.h"
#include "Utils/ImageLoader.h"
#include "Serialization/Buffer.h"

#define TEXTURE_CACHING 1

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

		MapBuffer map;
		MAPBUFFERWRITE(map, "m_width", { WRITE(&m_width, 4); });
		MAPBUFFERWRITE(map, "m_height", { WRITE(&m_height, 4); });
		MAPBUFFERWRITE(map, "m_Spec", { WRITE(&m_Spec, sizeof(TextureSpec)); });
		MAPBUFFERWRITE(map, "m_Pixels", { WRITEBUFFER(m_Pixels); });

		WRITEBUFFER(map.MakeBuffer());	
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		
		Buffer buf;
		READBUFFER(buf);
		MapBuffer map;
		map.FromBuffer(buf);
		MAPBUFFERREAD(map, "m_width", { READ(&m_width, 4); });
		MAPBUFFERREAD(map, "m_height", { READ(&m_height, 4); });
		MAPBUFFERREAD(map, "m_Spec", { READ(&m_Spec, sizeof(TextureSpec)); });
		MAPBUFFERREAD(map, "m_Pixels", { READBUFFER(m_Pixels); });

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

			//write pixels to file and clear the memory allocated by them currently
			if(TEXTURE_CACHING)
				CachePixels();

			return m_LoadedTexture;
		}
		else
			return m_LoadedTexture;
	}

	float GetWidth() const
	{
		return m_width;
	}

	float GetHeight() const
	{
		return m_height;
	}

	const TextureSpec& GetSpec() const
	{
		return m_Spec;
	}

	TextureSpec& GetSpecMutable()
	{
		return m_Spec;
	}

	Buffer LoadPixels()
	{
		if (m_PixelsFileStream.is_open())
		{
			Buffer out;
			out.resize(File::GetFileSize(m_CacheLocation));

			m_PixelsFileStream.seekg(0); //reset at 0 because reading moves the pointer to the end
			m_PixelsFileStream.read((char*)out.data(), out.size());
			return out;
		}
		else
			if (m_CacheLocation.empty() || !File::DoesPathExist(m_CacheLocation))
			{
				return m_Pixels;
			}
			else
			{
				m_PixelsFileStream = std::ifstream(m_CacheLocation, std::ios::in | std::ios::binary);
				return LoadPixels();
			}
	}

private:

	void CachePixels()
	{
		if(m_Pixels.size() < 1) return;

		File::MakePath(Paths::GetCacheDirectory() + "\\Textures\\");
		m_CacheLocation = Paths::GetCacheDirectory() + "\\Textures\\" + STRING(GetID().ID) + ".TextureCache";	
	
		File::WriteFile(m_CacheLocation, m_Pixels);
		m_PixelsFileStream = std::ifstream(m_CacheLocation, std::ios::in | std::ios::binary);
		m_Pixels = Buffer();
	}

	uint m_width = 1;
	uint m_height = 1;
	TextureSpec m_Spec;
	Buffer m_Pixels;
	Ref<Texture> m_LoadedTexture;

	//cache
	std::ifstream m_PixelsFileStream;
	std::string m_CacheLocation;
};