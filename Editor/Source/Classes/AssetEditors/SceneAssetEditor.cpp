#include "SceneAssetEditor.h"
#include "Classes/Viewport.h"

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

	m_SceneAsset = m_App->GetAssetManager().LoadAsset<SceneAsset>(m_TargetAsset);
	if(!m_SceneAsset) return;

	ImGui::Columns(2);

	//properties
	if (ImGui::Button("Save Current Scene"))
	{
		m_SceneAsset->SaveScene(m_App->m_EditorScene);
		m_App->GetAssetManager().SaveAsset(m_SceneAsset, m_TargetAsset);
		m_SceneAsset->LoadScene(m_Scene);
	}

	if (ImGui::Button("Set Base Class"))
	{
		ImGui::OpenPopup("Select Class");
	}
	if (ImGui::BeginPopup("Select Class"))
	{
		for (auto& key : m_App->GetObjectRegistry().GetRegisteredKeys())
		{
			if (key.type == ObjectClassType::APPOBJECT)
			{
				Ref<ObjectBase> test = ToRef(m_App->GetObjectRegistry().MakeObjectFromClassName(key.name));
				if (Cast<Scene>(test))
				{
					if (ImGui::MenuItem(key.name.c_str()))
					{
						m_SceneAsset->m_SceneClassName = key.name;
						m_App->GetAssetManager().SaveAsset(m_SceneAsset, m_TargetAsset);
						ImGui::CloseCurrentPopup();
					}
				}
			}		
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	std::string classText = "Scene Class : " + m_SceneAsset->m_SceneClassName;
	ImGui::Text(classText.c_str());

	if (ImGui::Button("Set Pipeline Class"))
	{
		ImGui::OpenPopup("Select pipeline Class");
	}
	if (ImGui::BeginPopup("Select pipeline Class"))
	{
		for (auto& key : m_App->GetObjectRegistry().GetRegisteredKeys())
		{
			if (key.type == ObjectClassType::OBJECT)
			{
				Ref<ObjectBase> test = ToRef(m_App->GetObjectRegistry().MakeObjectFromClassName(key.name));
				if (Cast<Pipeline>(test))
				{
					if (ImGui::MenuItem(key.name.c_str()))
					{
						m_SceneAsset->m_PipelineClassName = key.name;
						m_App->GetAssetManager().SaveAsset(m_SceneAsset, m_TargetAsset);
						ImGui::CloseCurrentPopup();
					}
				}
			}
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	std::string PipelineclassText = "Pipeline Class : " + m_SceneAsset->m_PipelineClassName;
	ImGui::Text(PipelineclassText.c_str());


	ImGui::NextColumn();

	//viewport
	m_App->GetWindow()->GetInputManager().ForwardTo(m_Viewport->m_InputManager);
	m_Viewport->BeginFrame();
	m_Scene->GetPipeline()->PrepareFrame();
	m_Viewport->EndFrame(false);
	m_Scene->GetPipeline()->ClearFrame();

	ImGui::Columns(1);

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
	m_Viewport = MakeRef<Viewport>(m_Scene, m_App->GetWindow(), "Scene View");
	m_SceneAsset = m_App->GetAssetManager().LoadAsset<SceneAsset>(m_TargetAsset);
	if (m_SceneAsset && !m_SceneAsset->m_SceneBuffer.empty())
	{
		m_SceneAsset->LoadScene(m_Scene);
	}
	{
		m_SceneAsset = MakeRef<SceneAsset>();
	}
}

