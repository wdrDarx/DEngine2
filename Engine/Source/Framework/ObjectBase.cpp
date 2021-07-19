#include "ObjectBase.h"
#include "Utils/Rand.h"

void ObjectBase::Initialize(const ObjectInitializer& initializer)
{
	//assign name (could be empty)
	m_Name = initializer.Name;

	//assign random ID
	if (initializer.Flags & ContructFlags::RANDOMID)
	{
		m_ID = { Rand::Int64() };
	}

	//start invalid
	if (initializer.Flags & ContructFlags::INVALIDATE)
	{
		m_IsValid = false;
	}

	//Define Properties
	DefineProperties();

	//call the virtual constructor
	OnConstruct();
}

uint ObjectBase::Serialize(Buffer& buffer)
{
	STARTWRITE(buffer, 0);
	WRITEBUFFER(GeneratePropBuffer());
	STOPWRITE()
}

uint ObjectBase::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, 0);
	Buffer PropBuffer;
	READBUFFER(PropBuffer);
	LoadPropsFromBuffer(PropBuffer);
	STOPREAD()
}

Buffer ObjectBase::GeneratePropBuffer() const
{
	ArrayBuffer propBuffer;
	for (const auto& prop : GetProperties())
	{
		if (prop.m_Flags & PropFlags::NoSerialize)
			continue;

		propBuffer.AddPiece(prop.MakeBuffer());
	}

	Buffer bf = propBuffer.MakeBuffer();
	return propBuffer.MakeBuffer();
}

void ObjectBase::LoadPropsFromBuffer(const Buffer& buffer)
{
	ArrayBuffer arrBuffer;
	arrBuffer.FromBuffer(buffer);

	for (auto& piece : arrBuffer.m_DataPieces)
	{
		Property NewProp;
		//need to deserialize new prop to get its name and type
		NewProp.LoadNameAndType(piece);

		//Go through old props and update them if a new one overrides it
		for (auto& oldProp : m_Properties)
		{
			//dont load if flagged not to serialize
			if(oldProp.m_Flags & PropFlags::NoSerialize)
				continue;

			//If a new prop didnt exist before then ignore it and dont laod it
			if (oldProp.m_name == NewProp.m_name && oldProp.m_Type == NewProp.m_Type)
			{
				//load the old prop with the new prop data
				oldProp.FromBuffer(piece);
				break;
			}
		}
	}
}


