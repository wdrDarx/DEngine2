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
	void LoadPrefab(Ref<SceneObject> prefab, bool LoadID = true, bool KeepRootTransform = true) const
	{
		LoadPrefab(prefab.get(), LoadID, KeepRootTransform);
	}

	/*
		loads in all saved object data (including properties) from this assets buffer
		Object must have initialized properties
	*/
	void LoadPrefab(SceneObject* prefab, bool LoadID = true, bool KeepRootTransform = true) const
	{
		if (m_SceneObjectBuffer.size() < 1) return;

		UID tempid;
		Transform temptransform;
		if(auto& root = prefab->GetRootComponent())
		{
			if(KeepRootTransform) //preserve root transform
				temptransform = root->GetLocalTransform();
		}

		if (LoadID)
			prefab->Deserialize(m_SceneObjectBuffer);
		else //preserve the object id
			tempid = prefab->GetID();

		//actually deserialize
		prefab->Deserialize(m_SceneObjectBuffer);

		if(!LoadID)
			prefab->SetID(tempid);

		if(KeepRootTransform)
			prefab->GetRootComponent()->SetLocalTransform(temptransform);
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
