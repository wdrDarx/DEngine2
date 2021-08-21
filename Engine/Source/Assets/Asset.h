#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"

/*
	inheritable class for serializing data into an asset, contains unique asset id 
*/

struct Texture;
struct AssetIcon
{
	void Serialize(Buffer& buffer) const;
	void Deserialize(const Buffer& buffer);
	Ref<Texture> GetTexture();

	uint m_width = 1;
	uint m_height = 1;
	TextureSpec m_Spec;
	Buffer m_Pixels;

	Ref<Texture> m_Texture = nullptr;
};

class DENGINE_API Asset
{
public:
	Asset();

	virtual uint Serialize(Buffer& buffer) const;

	virtual uint Deserialize(const Buffer& buffer);

	const UID& GetID() const
	{
		return m_ID;
	}

	//can be null
	Ref<AssetIcon> GetAssetIcon() const
	{
		return m_AssetIcon;
	}

private:
	//unique asset ID
	UID m_ID;

protected:
	Ref<AssetIcon> m_AssetIcon = nullptr;
};