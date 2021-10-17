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

		MapBuffer map;
		MAPBUFFERWRITE(map, "m_SceneClassName", { WRITESTRING(m_SceneClassName); });
		MAPBUFFERWRITE(map, "m_PipelineClassName", { WRITESTRING(m_PipelineClassName); });
		MAPBUFFERWRITE(map, "m_SceneBuffer", { WRITEBUFFER(m_SceneBuffer); });
	
		WRITEBUFFER(map.MakeBuffer());
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer) override
	{
		STARTREAD(buffer, Asset::Deserialize(buffer))
		
		Buffer buf;
		READBUFFER(buf);
		MapBuffer map(buf);
		MAPBUFFERREAD(map, "m_SceneClassName", { READSTRING(m_SceneClassName); });
		MAPBUFFERREAD(map, "m_PipelineClassName", { READSTRING(m_PipelineClassName); });
		MAPBUFFERREAD(map, "m_SceneBuffer", { READBUFFER(m_SceneBuffer); });

		STOPREAD();
	}

	void LoadScene(Ref<Scene> scene) const
	{
		LoadScene(scene.get());
	}

	void LoadScene(Scene* scene) const
	{
		if(scene && !m_SceneBuffer.empty())
		{
			scene->Deserialize(m_SceneBuffer);
		}
	}

	void SaveScene(Ref<Scene> scene)
	{
		scene->Serialize(m_SceneBuffer);
	}

	Buffer m_SceneBuffer;
	std::string m_SceneClassName;
	std::string m_PipelineClassName;
};
