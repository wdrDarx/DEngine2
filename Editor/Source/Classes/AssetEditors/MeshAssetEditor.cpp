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

	ImGui::Columns(2);

	if (ImGui::Button("Save"))
	{
		m_App->GetAssetManager().SaveAsset(m_MeshAsset, m_TargetAsset);
		OnMeshUpdate();
	}

	//properties
	if (ImGui::Button("Load From file"))
	{
		std::string meshPath = FileDialog::OpenFile("");
		Ref<Mesh> TempMesh = MakeRef<Mesh>();

		if(File::GetFileExtenstionFromPath(meshPath) == "obj")
			ModelLoader::LoadOBJ(meshPath, TempMesh.get());

		if (File::GetFileExtenstionFromPath(meshPath) == "fbx")
			ModelLoader::LoadFBX(meshPath, TempMesh.get());
			

		m_MeshAsset->m_Verticies = TempMesh->GetVerticies();
		m_MeshAsset->m_Indicies = TempMesh->GetIndicies();
		m_App->GetAssetManager().SaveAsset(m_MeshAsset, m_TargetAsset);
		OnMeshUpdate();
	}

	std::string assetText = "Vertex Count : " + STRING(m_MeshAsset->GetVerticies().size());
	ImGui::Text(assetText.c_str());

	assetText = "Index Count : " + STRING(m_MeshAsset->GetIndicies().size());
	ImGui::Text(assetText.c_str());

	ImGui::NextColumn();

	//viewport
	if (auto renderer = m_Scene->GetPipeline()->GetRenderer<DebugRenderer>())
		renderer->SetRenderFlags(m_App->m_DebugFlags);

	m_App->GetWindow()->GetInputManager().ForwardTo(m_Viewport->m_InputManager);
	m_Viewport->BeginFrame();
	m_Scene->GetPipeline()->PrepareFrame();
	m_Viewport->EndFrame(false);
	m_Scene->GetPipeline()->ClearFrame();

	ImGui::EndColumns();

	ImGui::End();
	if (!isOpen)
	{
		m_App->RemoveAssetEditor(this);
	}
}

void MeshAssetEditor::Init()
{
	m_MeshAsset = m_App->GetAssetManager().LoadAsset<MeshAsset>(m_TargetAsset);

	m_Scene = m_App->CreateAppObject<Scene>();
	m_Scene->SetPipeline<DefaultPipeline>(m_App->GetWindow()->GetRenderAPI());
	m_Viewport = MakeRef<Viewport>(m_Scene, m_App->GetWindow(), "Mesh View");



	m_PreviewMaterial = MakeRef<PBR_Material>();
	m_PreviewMaterial->Initialize(ObjectInitializer());
	m_PreviewMaterial->SetSceneContext(m_Scene.get());
	m_PreviewMaterial->Albedo = { 1,1,1,1 };
	m_PreviewMaterial->Roughness = 0.2f;
	m_PreviewMaterial->Metallic = 0.0f;


	m_Light = MakeRef<DirectionalLight>();
	m_Light->Direction = {0.5, -0.7, 0.0};
	m_Light->Radiance = {1,1,1};
	m_Light->CastShadows = false;
	m_Scene->GetPipeline()->GetRenderer<MeshRenderer>()->SubmitDirectionalLight(m_Light);

	OnMeshUpdate();
}

void MeshAssetEditor::OnMeshUpdate()
{
	if (m_MeshAsset && m_MeshAsset->GetVerticies().size() > 0)
	{
		if(m_PreviewMesh)
			m_Scene->GetPipeline()->GetRenderer<MeshRenderer>()->RemoveMesh(m_PreviewMesh);

		m_PreviewMesh = MakeRef<Mesh>();
		m_PreviewMesh->SetData(m_MeshAsset->GetVerticies(), m_MeshAsset->GetIndicies());
		m_PreviewMesh->SetMaterial(m_PreviewMaterial);
		m_Scene->GetPipeline()->GetRenderer<MeshRenderer>()->SubmitMesh(m_PreviewMesh);
	}
}


