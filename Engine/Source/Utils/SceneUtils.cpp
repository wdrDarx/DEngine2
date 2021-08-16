#include "SceneUtils.h"
#include "DEngine.h"

Ref<SceneObject> SceneUitls::SpawnPrefabInScene(AssetRef<PrefabAsset> prefabAsset, Ref<Scene> scene, const Transform& transform)
{
	auto loadedPrefab = scene->GetApplication()->GetAssetManager().LoadAsset(prefabAsset);
	auto sceneObject = ToRef<SceneObject>(Cast<SceneObject>(scene->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(loadedPrefab->GetPrefabBaseClassName())));
	//ASSERT(sceneObject); //object class invalid or not registered
	if (!sceneObject) return nullptr;

	scene->AddSceneObject(sceneObject);

	//load the prefab data
	loadedPrefab->LoadPrefab(sceneObject, false);

	//mark a prefab
	sceneObject->MarkPrefab(prefabAsset);

	return sceneObject;
}

void SceneUitls::LoadSceneFromAsset(Ref<SceneAsset> sceneAsset, Ref<Scene> scene)
{
	if (!scene) return;
	scene->DestroyAllSceneObjects();
	scene->GetInputManager().ClearInput();
	scene->GetSceneEventDipatcher().UnbindAll();

	sceneAsset->LoadScene(scene);
}
