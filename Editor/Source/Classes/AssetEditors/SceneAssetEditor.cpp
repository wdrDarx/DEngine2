#include "SceneAssetEditor.h"

SceneAssetEditor::SceneAssetEditor() : AssetEditor("PrefabAsset")
{
	
}

SceneAssetEditor::~SceneAssetEditor()
{
	m_App->DestroyAppObject(m_Scene);
}

void SceneAssetEditor::Render()
{
	title = m_TargetAsset->GetAssetName() + " Scene Editor";
	bool isOpen = true;
	ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoDocking);

	//properties
	if (ImGui::Button("Save Current Scene"))
	{
		m_SceneAsset->SaveScene(m_App->m_EditorScene);
		m_App->GetAssetManager().SaveAsset(m_SceneAsset, m_TargetAsset);
		m_SceneAsset->LoadScene(m_Scene);
	}

	ImGui::Columns(1);

	//viewport
	m_App->GetWindow()->GetInputManager().ForwardTo(m_Viewport->m_InputManager);
	m_Viewport->BeginFrame();
	m_Scene->GetPipeline()->PrepareFrame();
	m_Viewport->EndFrame(false);
	m_Scene->GetPipeline()->ClearFrame();

	ImGui::End();
	if (!isOpen)
	{
		m_App->RemoveAssetEditor(this);
	}
}

void SceneAssetEditor::Init()
{
	m_Scene = m_App->CreateAppObject<Scene>();
	m_Scene->SetPipeline<DefaultPipeline>(m_App->GetWindow()->GetRenderAPI());
	m_Viewport = MakeRef<Viewport>(m_Scene, m_App->GetWindow(), nullptr, "Scene View");
	m_SceneAsset = m_App->GetAssetManager().LoadAsset<SceneAsset>(m_TargetAsset);
	if (m_SceneAsset && !m_SceneAsset->m_SceneBuffer.empty())
	{
		m_SceneAsset->LoadScene(m_Scene);
	}
	{
		m_SceneAsset = MakeRef<SceneAsset>();
	}
}

