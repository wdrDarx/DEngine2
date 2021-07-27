#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES TestAsset : public Asset
{
public:
	TestAsset() : Asset()
	{

	}

	~TestAsset()
	{
		
	}

	TestAsset(const std::string& msg) : m_Message(msg)
	{

	}


	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITESTRING(m_Message);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READSTRING(m_Message);
		STOPREAD();
	}

	std::string m_Message;
};