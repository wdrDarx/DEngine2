#include "Asset.h"
#include "Utils/Rand.h"

Asset::Asset()
{
	//assign random id on construct (will get overriden on deserialize)
	m_ID.ID = Rand::Int64();
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
