#include "SceneUtils.h"
#include "DEngine.h"

Ref<SceneObject> SceneUitls::SpawnPrefabInScene(AssetRef<PrefabAsset> prefabAsset, Ref<Scene> scene, const Transform& transform)
{
	auto loadedPrefab = scene->GetApplication()->GetAssetManager().LoadAsset(prefabAsset);
	auto sceneObject = ToRef<SceneObject>(Cast<SceneObject>(scene->GetApplication()->GetObjectRegistry().MakeObjectFromClassName(loadedPrefab->GetPrefabBaseClassName())));
	//ASSERT(sceneObject); //object class invalid or not registered
	if (!sceneObject) return nullptr;

	//initialize the object calling DefineProperties and OnConstruct so all the props and components are there 
	scene->AddSceneObject(sceneObject, ObjectInitializer(ConstructFlags::NOPOSTCONSTRUCT | ConstructFlags::RANDOMID));

	//load the prefab data
	loadedPrefab->LoadPrefab(sceneObject, false);

	//mark a prefab
	sceneObject->MarkPrefab(prefabAsset);

	//and now after being deserialized call on post construct 
	sceneObject->OnPostConstruct();

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
