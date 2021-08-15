#include "SceneUtils.h"
#include "DEngine.h"

Ref<SceneObject> SceneUitls::SpawnPrefabInScene(Ref<PrefabAsset> prefabAset, Ref<Scene> scene, const Transform& transform)
{
	auto sceneObject = ToRef<SceneObject>(Cast<SceneObject>(scene->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(prefabAset->GetPrefabBaseClassName())));
	//ASSERT(sceneObject); //object class invalid or not registered
	if (!sceneObject) return nullptr;

	scene->AddSceneObject(sceneObject);

	//load the prefab data
	prefabAset->LoadPrefab(sceneObject, false);

	//mark a prefab
	sceneObject->MarkPrefab(prefabAset);

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
