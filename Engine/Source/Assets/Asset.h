#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"

/*
	inheritable class for serializing data into an asset, contains unique asset id 
*/
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

private:
	//unique asset ID
	UID m_ID;
};