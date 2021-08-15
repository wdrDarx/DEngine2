#pragma once
#include "Assets/Asset.h"
#include "Framework/Scene.h"
#include "Framework/SceneObject.h"
#include "Serialization/Buffer.h"

class DENGINE_API SceneAsset : public Asset
{
public:
	SceneAsset() : Asset()
	{

	}

	~SceneAsset()
	{

	}

	uint Serialize(Buffer& buffer) const override
	{
		STARTWRITE(buffer, Asset::Serialize(buffer))
		WRITEBUFFER(m_SceneBuffer);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		READBUFFER(m_SceneBuffer);
		STOPREAD();
	}

	void LoadScene(Ref<Scene> scene) const
	{
		LoadScene(scene.get());
	}

	void LoadScene(Scene* scene) const
	{
		if(scene)
		{
			scene->Deserialize(m_SceneBuffer);
		}
	}

	void SaveScene(Ref<Scene> scene)
	{
		scene->Serialize(m_SceneBuffer);
	}

	Buffer m_SceneBuffer;
};
