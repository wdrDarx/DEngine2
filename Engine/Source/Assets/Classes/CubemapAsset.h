#pragma once
#include "Assets/Asset.h"
#include "Rendering/Texture.h"
#include "Utils/ImageLoader.h"
#include "Serialization/Buffer.h"

class DENGINE_API CubemapAsset : public Asset
{
public:
	CubemapAsset() : Asset()
	{

	}

	~CubemapAsset()
	{

	}

	CubemapAsset(const Image& image)
	{
		
	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITE(&m_width, 4);
		WRITE(&m_height, 4);
		WRITEVEC(m_Pixels, 4);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READ(&m_width, 4);
		READ(&m_height, 4);
		READVEC(m_Pixels, 4);
		STOPREAD();
	}

	uint m_width = 1;
	uint m_height = 1;
	std::vector<float> m_Pixels;
};