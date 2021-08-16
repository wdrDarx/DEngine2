#pragma once
#include "Core/Core.h"
#include "Framework/Scene.h"
#include "Framework/SceneObject.h"
#include "Assets/Classes/PrefabAsset.h"
#include "Assets/Classes/SceneAsset.h"
#include "Assets/AssetRef.h"


/*
	Util class for scene actions
*/
class SceneUitls
{
public:

	/*
		Loads a prefab from an asset and spawns it in the specified scene at the speciefied transform
	*/
	static Ref<SceneObject> SpawnPrefabInScene(AssetRef<PrefabAsset> prefabAsset, Ref<Scene> scene, const Transform& transform);

	/*
		Wipes a scene fully before loading in all the data from the asset
	*/
	static void LoadSceneFromAsset(Ref<SceneAsset> sceneAsset, Ref<Scene> scene);

};