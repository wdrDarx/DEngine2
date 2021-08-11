#pragma once
#include "Core/Core.h"
#include "Framework/Scene.h"
#include "Framework/SceneObject.h"
#include "Assets/Classes/PrefabAsset.h"

/*
	Util class for scene actions
*/
class SceneUitls
{
public:

	/*
		Loads a prefab from an asset and spawns it in the specified scene at the speciefied transform
	*/
	static Ref<SceneObject> SpawnPrefabInScene(Ref<PrefabAsset> prefabAset, Ref<Scene> scene, const Transform& transform);

};