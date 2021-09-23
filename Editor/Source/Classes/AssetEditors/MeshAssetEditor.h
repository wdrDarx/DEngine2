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

	void OnMeshUpdate();


	Ref<Viewport> m_Viewport;
	Ref<Scene> m_Scene;
	Ref<MeshAsset> m_MeshAsset;
	Ref<Mesh> m_PreviewMesh = nullptr;
	Ref<PBR_Material> m_PreviewMaterial = nullptr;
	Ref<DirectionalLight> m_Light = nullptr;
	std::string title;
};
