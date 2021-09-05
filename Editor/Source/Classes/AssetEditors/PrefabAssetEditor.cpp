#include "PrefabAssetEditor.h"

PrefabAssetEditor::PrefabAssetEditor() : AssetEditor("PrefabAsset")
{

}

PrefabAssetEditor::~PrefabAssetEditor()
{
	m_App->DestroyAppObject(m_Scene);
}

void PrefabAssetEditor::Init()
{
	m_PrefabAsset = m_App->GetAssetManager().LoadAsset<PrefabAsset>(m_TargetAsset);

	if (!m_PrefabAsset) return;

	m_Scene = m_App->CreateAppObject<Scene>();
	m_Scene->SetPipeline<DefaultPipeline>(m_App->GetWindow()->GetRenderAPI());
	m_Viewport = MakeRef<Viewport>(m_Scene, m_App->GetWindow(), "Prefab View");

	m_PropertyWindow.Init(m_App);


	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED)
		{
			if (m_SceneObject->GetAssociatedModuleName() == event->m_ModuleName)
			{
				m_App->RemoveAssetEditor(this);
			}
		}
	
	});

	m_App->GetEventDispatcher().Bind(m_ModuleCallback);
	

	if(!m_PrefabAsset->GetPrefabBaseClassName().empty())
		UpdatePrefabClass();
}

void PrefabAssetEditor::UpdatePrefabClass()
{
	//create the scene object from the class
	if(m_SceneObject)
		m_Scene->DestroySceneObject(m_SceneObject);

	//create the new scene object from the correct class
	auto obj = m_App->GetObjectRegistry().MakeObjectFromClassName(m_PrefabAsset->GetPrefabBaseClassName());
	if(!obj)
	{
		return;
	}

	m_SceneObject = ToRef<SceneObject>(Cast<SceneObject>(obj));
	m_Scene->AddSceneObject(m_SceneObject, ObjectInitializer(ConstructFlags::RANDOMID | ConstructFlags::NOPOSTCONSTRUCT));

	//load the prefab data
	m_PrefabAsset->LoadPrefab(m_SceneObject, false);

	//call post construct
	m_SceneObject->OnPostConstruct();
}

void PrefabAssetEditor::Render()
{
	m_PrefabAsset = m_App->GetAssetManager().LoadAsset<PrefabAsset>(m_TargetAsset);
	if(!m_PrefabAsset) return;

	title = m_TargetAsset->GetAssetName() + " Prefab Editor";
	bool isOpen = true;
	ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoDocking);

	if (ImGui::Button("Save"))
	{
		m_SceneObject->MarkPrefab(AssetRef<PrefabAsset>(m_TargetAsset->GetAssetPath()));
		m_PrefabAsset->SavePrefab(m_SceneObject);
		m_App->GetAssetManager().SaveAsset(m_PrefabAsset, m_TargetAsset);
	}

	ImGui::Columns(3);
	ImGui::BeginChild("Properties");

	//Object pannel
	m_SceneObjectPannel.Render(m_Scene.get(), false);

	//properties
	m_PropertyWindow.m_SelectedComponent = m_SceneObjectPannel.m_SelectedComponent;
	m_PropertyWindow.m_SelectedSceneObject = m_SceneObjectPannel.m_SelectedObject;

	bool expanded = ImGui::TreeNodeEx((void*)(m_SceneObjectPannel.m_SelectedComponent), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen, "Properties");
	if(expanded)
	{ 
		m_PropertyWindow.Render(false);
		ImGui::TreePop();
	}
	ImGui::EndChild();

	ImGui::NextColumn();


	//viewport
	if(auto renderer = m_Scene->GetPipeline()->GetRenderer<DebugRenderer>())
		renderer->SetRenderFlags(m_App->m_DebugFlags);

	m_App->GetWindow()->GetInputManager().ForwardTo(m_Viewport->m_InputManager);
	m_Viewport->m_SelectedComponent = m_SceneObjectPannel.m_SelectedComponent;
	m_Viewport->m_TransformMode = m_App->m_TransformMode;
	m_Viewport->BeginFrame();
	m_Scene->GetPipeline()->PrepareFrame();
	m_Viewport->EndFrame(false);
	m_Scene->GetPipeline()->ClearFrame();
	ImGui::NextColumn();

	//defaults
	if (ImGui::Button("Set Base Class"))
	{
		ImGui::OpenPopup("Select Asset");
	}

	if (ImGui::BeginPopup("Select Asset"))
	{
		for (auto& key : m_App->GetObjectRegistry().GetRegisteredKeys())
		{
			if (key.type == ObjectClassType::SCENEOBJECT)
			{
				if (ImGui::MenuItem(key.name.c_str()))
				{
					m_PrefabAsset->m_SceneObjectClassName = key.name;
					m_App->GetAssetManager().SaveAsset(m_PrefabAsset, m_TargetAsset);
					UpdatePrefabClass();
					ImGui::CloseCurrentPopup();
				}
			}
		}

		ImGui::EndPopup();
	}
	
	std::string assetText =  "Base Class : " + m_PrefabAsset->GetPrefabBaseClassName();
	ImGui::Text(assetText.c_str());
	ImGui::Columns(1);
	//end

	ImGui::End();
	if (!isOpen)
	{ 
		m_App->RemoveAssetEditor(this);
	}
}


