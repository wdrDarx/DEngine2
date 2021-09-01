#include "ObjectBase.h"
#include "Utils/Rand.h"

void ObjectBase::Initialize(const ObjectInitializer& initializer)
{
	//assign the member initializer for later use
	m_ObjectInitializer = initializer;

	//flag initialized as true
	m_Initialized = true;

	//assign name, class name by default
	if (initializer.Name.empty())
		m_Name = GetClassType().Name;
	else
		m_Name = initializer.Name;

	//override AssociatedModuleName 
	if(!initializer.AssociatedModuleName.empty())
		SetAssociatedModuleName(initializer.AssociatedModuleName);

	//assign random ID
	if (initializer.Flags & ConstructFlags::RANDOMID)
	{
		m_ID = { Rand().Int64() };
	}

	//start invalid
	if (initializer.Flags & ConstructFlags::INVALIDATE)
	{
		m_IsValid = false;
	}

	//Define Properties
	if ((initializer.Flags & ConstructFlags::NOPROPS) == 0) //if the flag is not set
		DefineProperties();

	//call the virtual constructor
	if((initializer.Flags & ConstructFlags::NOCONSTRUCT) == 0) //if the flag is not set
		OnConstruct();

	//call the virtual post construct by default (this will usually disabled and called manually after deserialzation)
	if ((initializer.Flags & ConstructFlags::NOPOSTCONSTRUCT) == 0) //if the flag is not set
		OnPostConstruct();
}

uint ObjectBase::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, 0);

	WRITE(&m_ID, sizeof(UID));
	WRITESTRING(m_Name);
	WRITE(&m_ObjectFlags, sizeof(ObjectFlags));

	WRITEBUFFER(GeneratePropBuffer());
	STOPWRITE()
}

uint ObjectBase::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, 0);

	READ(&m_ID, sizeof(UID));
	READSTRING(m_Name);
	READ(&m_ObjectFlags, sizeof(ObjectFlags));

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


ObjectInitializer ObjectInitializer::Module(ObjectBase* ExistantModuleObject)
{
	ASSERT(ExistantModuleObject);

	ObjectInitializer init;
	init.AssociatedModuleName = ExistantModuleObject->GetAssociatedModuleName();
	return init;
}

ObjectInitializer ObjectInitializer::Module(const std::string& AssociatedModuleName)
{
	ObjectInitializer init;
	init.AssociatedModuleName = AssociatedModuleName;
	return init;
}
