#include "MeshAssetEditor.h"

MeshAssetEditor::MeshAssetEditor() : AssetEditor("Mesh Editor")
{

}

void MeshAssetEditor::Render()
{
	m_MeshAsset = m_App->GetAssetManager().LoadAsset<MeshAsset>(m_TargetAsset);
	if (!m_MeshAsset) return;

	title = m_TargetAsset->GetAssetName() + " Mesh Editor";
	bool isOpen = true;
	ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoDocking);

	if (ImGui::Button("Save"))
	{
		m_App->GetAssetManager().SaveAsset(m_MeshAsset, m_TargetAsset);
	}

	//properties
	if (ImGui::Button("Load From file"))
	{
		std::string meshPath = FileDialog::OpenFile("");
		Ref<Mesh> TempMesh = MakeRef<Mesh>();
		ModelLoader::LoadOBJ(meshPath, TempMesh.get());

		m_MeshAsset->m_Verticies = TempMesh->GetVerticies();
		m_MeshAsset->m_Indicies = TempMesh->GetIndicies();
		m_App->GetAssetManager().SaveAsset(m_MeshAsset, m_TargetAsset);
	}

	std::string assetText = "Vertex Count : " + STRING(m_MeshAsset->GetVerticies().size());
	ImGui::Text(assetText.c_str());

	assetText = "Index Count : " + STRING(m_MeshAsset->GetIndicies().size());
	ImGui::Text(assetText.c_str());


	ImGui::End();
	if (!isOpen)
	{
		m_App->RemoveAssetEditor(this);
	}
}

void MeshAssetEditor::Init()
{

}

void MeshAssetEditor::UpdateObjectClass()
{

}
