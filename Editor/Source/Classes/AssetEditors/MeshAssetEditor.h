#pragma once
#include "Classes/AssetEditor.h"
#include "Classes/EditorApp.h"
#include "DEngine.h"

class MeshAssetEditor : public AssetEditor
{
public:
	MeshAssetEditor();

	void Render() override;
	void Init() override;

	void UpdateObjectClass();

	PropertyWindow m_PropertyWindow;
	Ref<MeshAsset> m_MeshAsset;
	std::string title;
};
