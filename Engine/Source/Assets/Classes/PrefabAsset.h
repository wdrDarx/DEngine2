#pragma once
#include "Assets/Asset.h"
#include "Framework/SceneObject.h"
#include "Serialization/Buffer.h"

class DENGINE_API PrefabAsset : public Asset
{
public:
	PrefabAsset() : Asset()
	{

	}

	~PrefabAsset()
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITESTRING(m_SceneObjectClassName);
		WRITEBUFFER(m_SceneObjectBuffer);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READSTRING(m_SceneObjectClassName);
		READBUFFER(m_SceneObjectBuffer);
		STOPREAD();
	}

	/*
		loads in all saved object data (including properties) from this assets buffer
	 	Object must have initialized properties
	*/
	void LoadPrefab(Ref<SceneObject> prefab, bool LoadID = true) const
	{
		LoadPrefab(prefab.get(), LoadID);
	}

	/*
		loads in all saved object data (including properties) from this assets buffer
		Object must have initialized properties
	*/
	void LoadPrefab(SceneObject* prefab, bool LoadID = true) const
	{
		if (m_SceneObjectBuffer.size() < 1) return;

		if (LoadID)
			prefab->Deserialize(m_SceneObjectBuffer);
		else //preserve the object id
		{
			UID tempid = prefab->GetID();
			prefab->Deserialize(m_SceneObjectBuffer);
			prefab->SetID(tempid);
		}
	}

	void SavePrefab(Ref<SceneObject> prefab)
	{
		prefab->Serialize(m_SceneObjectBuffer);
	}

	const std::string GetPrefabBaseClassName() const
	{
		return m_SceneObjectClassName;
	}

	std::string m_SceneObjectClassName;
	Buffer m_SceneObjectBuffer;
};
