#pragma once
#include "Assets/Asset.h"
#include "Framework/ObjectBase.h"
#include "Framework/Registry.h"
#include "Utils/ObjectUtils.h"
#include "Serialization/Buffer.h"

/*
	Inheritable asset class that just serializes an objects properties
*/
class DENGINE_API ObjectAsset : public Asset
{
public:
	ObjectAsset() : Asset()
	{

	}

	~ObjectAsset()
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITESTRING(m_ObjectClassName);
		WRITEBUFFER(m_PropsBuffer);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READSTRING(m_ObjectClassName);
		READBUFFER(m_PropsBuffer);
		STOPREAD();
	}

	void SaveProps(const std::vector<StaticProperty>& props)
	{
		m_PropsBuffer = ObjectUtils::GenerateStaticPropsBuffer(props);
	}

	std::vector<StaticProperty> LoadProps(StructRegistry& structRegistry) const
	{
		return ObjectUtils::GenerateStaticPropsFromBuffer(m_PropsBuffer, structRegistry);
	}

	void SaveObjectProps(Ref<ObjectBase> obj)
	{
		ASSERT(obj);
		//actually cheeky af hack
		m_PropsBuffer = ObjectUtils::GenerateStaticPropsBuffer(*(std::vector<StaticProperty>*)&obj->GetProperties());
		m_ObjectClassName = obj->GetClassType().Name;
	}

	void LoadObjectProps(Ref<ObjectBase> obj)
	{
		ASSERT(obj);
		ASSERT(obj->GetClassType().Name == m_ObjectClassName); //Class missmatch!
		obj->LoadPropsFromBuffer(m_PropsBuffer);
	}

	const std::string& GetObjectClassName() const
	{
		return m_ObjectClassName;
	}

	//very careful with this one
	void SetObjectClass(const std::string& ClassName)
	{
		m_ObjectClassName = ClassName;
	}

	void SetObjectClass(Ref<ObjectBase> obj)
	{
		m_ObjectClassName = obj->GetClassType().Name;
	}

	//not necessary if you just need to serialize static properties
	std::string m_ObjectClassName;
	Buffer m_PropsBuffer;
};
