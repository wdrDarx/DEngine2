#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"
#include "Utils/Rand.h"

class DENGINE_API Asset
{
public:
	Asset();

	virtual uint Serialize(Buffer& buffer) const
	{
		STARTWRITE(buffer, 0);
		WRITE(&m_ID.ID, sizeof(uint64));
		STOPWRITE();
	}

	virtual uint Deserialize(const Buffer& buffer)
	{
		STARTREAD(buffer, 0);
		READ(&m_ID, sizeof(UID));
		STOPREAD();
	}

	//unique asset ID
	UID m_ID;
};