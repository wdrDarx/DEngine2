#include "Asset.h"
#include "Rendering/Texture.h"
#include "Utils/Rand.h"

Asset::Asset()
{
	//assign random id on construct (will get overriden on deserialize)
	m_ID.ID = Rand().Int64();
	m_AssetIcon = nullptr;
}

uint Asset::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, 0);
	WRITE(&m_ID.ID, sizeof(uint64));
	STOPWRITE();
}

uint Asset::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, 0);
	READ(&m_ID, sizeof(UID));
	STOPREAD();
}

void AssetIcon::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, 0);
	WRITE(&m_width, sizeof(uint));
	WRITE(&m_height, sizeof(uint));
	WRITE(&m_Spec, sizeof(TextureSpec));
	WRITEBUFFER(m_Pixels);
}

void AssetIcon::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, 0);
	READ(&m_width, sizeof(uint));
	READ(&m_height, sizeof(uint));
	READ(&m_Spec, sizeof(TextureSpec));
	READBUFFER(m_Pixels);
}

Ref<Texture> AssetIcon::GetTexture()
{
	if (m_Texture)
	{
		return m_Texture;
	}
	else
	{
		m_Texture = MakeRef<Texture>(m_Spec, m_width, m_height, m_Pixels.data());
		return m_Texture;
	}
}
