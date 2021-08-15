#pragma once
#include "Classes/AssetEditor.h"
#include "Classes/EditorApp.h"
#include "DEngine.h"

class SceneAssetEditor : public AssetEditor
{
public:
	SceneAssetEditor();
	~SceneAssetEditor();

	void Render() override;
	void Init() override;

	Ref<SceneAsset> m_SceneAsset;
	std::string title;

	Ref<Viewport> m_Viewport;
	Ref<Scene> m_Scene;
};
