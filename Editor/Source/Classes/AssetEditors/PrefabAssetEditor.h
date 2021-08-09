#pragma once
#include "Classes/AssetEditor.h"
#include "Classes/EditorApp.h"
#include "DEngine.h"

class PrefabAssetEditor : public AssetEditor
{
public:
	PrefabAssetEditor();

	~PrefabAssetEditor();

	void Render() override;
	void Init() override;

	void UpdatePrefabClass();


	Ref<PrefabAsset> m_PrefabAsset;
	std::string title;
	Ref<Viewport> m_Viewport;

	Ref<Scene> m_Scene;
	Ref<SceneObject> m_SceneObject;

	SceneObjectPannel m_SceneObjectPannel;
	PropertyWindow m_PropertyWindow;
};

