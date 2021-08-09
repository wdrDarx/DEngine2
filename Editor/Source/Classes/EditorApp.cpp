#include "EditorApp.h"
#include "Event/Event.h"

//asset editors
#include "Classes/AssetEditors/TextureAssetEditor.h"
#include "Classes/AssetEditors/PrefabAssetEditor.h"

EditorApp::EditorApp() : Application()
{
	MakeWindow("DEditor", 1280, 720, false);
	GetWindow()->SetVsync(false);

	//application event for end frame on the scene
	m_ApplicationEvent.Assign([&](ApplicationEvent* event)
	{
		if (event->GetEventType() == ApplicationEventType::OBJECTFINSIHUPDATE)
		{
			EndFrame();
		}
	});

	//close event for window
	m_WindowEvent.Assign([&](WindowEvent* event)
	{
		if (event->GetEventType() == WindowEventType::CLOSED)
		{
			m_ImGuiLayer.Shutdown();
		}
	});

	//module events
	m_ModuleEvent.Assign([&](ModuleEvent* event)
	{
		if(event->GetEventType() == ModuleEventType::LOADED)
			LogTemp("LOADED : " + event->m_ModuleName);

		if (event->GetEventType() == ModuleEventType::UNLOADED)
			LogTemp("UNLOADED : " + event->m_ModuleName);
	});

	//Scene events (Auto unissign for property and pannel windows)
	m_SceneEvent.Assign([&](SceneEvent* event)
	{
		//Auto unassign selected scene object in property window and scene pannel if that object/component was deleted
		if (event->GetEventType() == SceneEventType::OBJECT_PRE_DELETE)
		{
			if (event->GetSceneObject() && m_PropertyWindow.m_SelectedSceneObject.get() == event->GetSceneObject())
			{
				m_PropertyWindow.m_SelectedSceneObject = nullptr;
			}

			if (event->GetSceneObject() && m_SceneObjectPannel.m_SelectedObject.get() == event->GetSceneObject())
			{
				m_SceneObjectPannel.m_SelectedObject = nullptr;
			}
		} 

		if (event->GetEventType() == SceneEventType::COMPONENT_PRE_DELETE)
		{
			if (m_PropertyWindow.m_SelectedComponent.get() == event->GetComponent())
			{
				m_PropertyWindow.m_SelectedComponent = nullptr;
			}

			if (m_SceneObjectPannel.m_SelectedComponent.get() == event->GetComponent())
			{
				m_SceneObjectPannel.m_SelectedComponent = nullptr;
			}
		}
	});

	//bind the application event callback 
	GetEventDispatcher().Bind(m_ApplicationEvent);

	//bind the window event callback 
	GetEventDispatcher().Bind(m_WindowEvent);

	//bind the module event callback
	GetEventDispatcher().Bind(m_ModuleEvent);

	//load all modules
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());

	//here to make sure any loaded modules could load their asset types
	GetAssetManager().DiscoverAssets();

	//init imgui layer
	m_ImGuiLayer.Init(GetWindow());

	//Make the scene
	m_EditorScene = CreateAppObject<Scene>();

	//bind scene events
	m_EditorScene->GetSceneEventDipatcher().Bind(m_SceneEvent);

	//property window
	m_PropertyWindow.Init(this);

	//content browser
	m_ContentBrowser.Init();

	//main viewport
	CreateViewport(m_EditorScene);

	//bind key event to input manager
	m_OnKeyDownCallback.Assign(std::bind(&EditorApp::OnKeyDown, this, std::placeholders::_1));
	GetWindow()->GetInputManager().BindOnKeyDown(m_OnKeyDownCallback);
}

void EditorApp::OnUpdate(const Tick& tick)
{
	BeginFrame();

	if (m_ImGuiLayer.IsValid())
	{
		RenderImGui(tick);
	}
}

void EditorApp::RenderImGui(const Tick& tick)
{
	m_MenuBar.Render(this);
	m_ContentBrowser.Render(this);

	m_SceneObjectPannel.Render(m_EditorScene);
	if (m_SceneObjectPannel.m_SelectedComponent)
	{
		m_PropertyWindow.m_SelectedSceneObject = nullptr;
		m_PropertyWindow.m_SelectedComponent = m_SceneObjectPannel.m_SelectedComponent;
	}
	else if (m_SceneObjectPannel.m_SelectedObject)
	{
		m_PropertyWindow.m_SelectedSceneObject = m_SceneObjectPannel.m_SelectedObject;
		m_PropertyWindow.m_SelectedComponent = nullptr;
	}
	m_PropertyWindow.Render();

	ImGui::Begin("Application Objects");

	for (auto& obj : GetAppObjects())
	{
		ImGui::Text(obj->GetClassType().Name.c_str());
	}

	ImGui::End();

	ImGui::Begin("Registry");

	if (ImGui::TreeNode("Structs"))
	{
		for (auto& reg : GetStructRegistry().GetRegisteredKeys())
		{
			if (ImGui::Button(reg.name.c_str()))
			{
				//(GetStructRegistry().Make(reg));
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Objects"))
	{
		for (auto& reg : GetObjectRegistry().GetRegisteredKeys())
		{
			if (reg.type == ObjectClassType::OBJECT)
			{
				if (ImGui::Button(reg.name.c_str()))
				{
					(GetObjectRegistry().Make(reg))->Initialize(ObjectInitializer());
				}
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("App Objects"))
	{
		for (auto& reg : GetObjectRegistry().GetRegisteredKeys())
		{
			if (reg.type == ObjectClassType::APPOBJECT)
			{
				if (ImGui::Button(reg.name.c_str()))
				{
					AddAppObject(Cast<AppObject>(GetObjectRegistry().Make(reg)));
				}
			}
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SceneObjects"))
	{
		for (auto& reg : GetObjectRegistry().GetRegisteredKeys())
		{
			if (reg.type == ObjectClassType::SCENEOBJECT)
			{
				if (ImGui::Button(reg.name.c_str()))
				{
					m_EditorScene->AddSceneObject(Cast<SceneObject>(GetObjectRegistry().Make(reg)));
				}
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Components"))
	{
		for (auto& reg : GetObjectRegistry().GetRegisteredKeys())
		{
			if (reg.type == ObjectClassType::OBJECTCOMPONENT)
			{
				if (ImGui::Button(reg.name.c_str()))
				{

				}
			}
		}

		ImGui::TreePop();
	}

	ImGui::End();

	ImGui::Begin("Asset Manager");

	if (ImGui::TreeNode("AssetTypes"))
	{
		for (auto& type : GetAssetManager().GetAssetTypeRegistry().GetRegisteredKeys())
		{
			ImGui::Text(type.name.c_str());
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Discover Assets"))
	{
		GetAssetManager().DiscoverAssets();
	}

	if (ImGui::TreeNode("Discovered Assets"))
	{
		for (auto& asset : GetAssetManager().GetAllDiscorveredAssets())
		{
			std::string display = "(" + asset.GetAssetType() + ") " + asset.GetAssetName() + " (" + STRING(asset.GetAssetID().ID) + ")";
			ImGui::Text(display.c_str());
		}
		ImGui::TreePop();
	}


	ImGui::End();

	ImGui::Begin("Modules");

	if (ImGui::Button("Reload All"))
	{
		GetModuleManager().UnloadAllModules();
		GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());
	}

	for (const auto& mod : GetModuleManager().GetLoadedModules())
	{
		ImGui::Text(mod->m_Name.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Unload"))
		{
			GetModuleManager().UnloadModule(mod->m_Name);
		}
	}

	ImGui::End();

	ImGui::Begin("Render Stats");
	ImGui::Text(std::string("FPS : " + STRING(int(1.0f / tick.DeltaTime))).c_str());
	ImGui::End();
}

void EditorApp::BeginFrame()
{
	GetWindow()->StartFrame();
	m_ImGuiLayer.Begin();
	if (!m_ImGuiLayer.IsValid()) return;

	//begin frame for all viewports (requires imGui context)
	bool InputSet = false;
	for (auto& viewport : m_Viewports)
	{
		//forward input to viewport
		if(!InputSet)
		{ 
			if(viewport->m_Scene)
			{
				GetWindow()->GetInputManager().StopForwarding(viewport->m_Scene->GetInputManager());

				//for camera movement and stuff
				GetWindow()->GetInputManager().StopForwarding(viewport->m_InputManager);
				
			}
			if (viewport->m_Scene && viewport->m_FocusedOnviewport)
			{
				GetWindow()->GetInputManager().ForwardTo(viewport->m_Scene->GetInputManager());

				//for camera movement and stuff
				GetWindow()->GetInputManager().ForwardTo(viewport->m_InputManager);
				InputSet = true;
			}
		}

		//begin frame for viewport
		viewport->BeginFrame();		
	}
}

void EditorApp::EndFrame()
{
	if (!m_ImGuiLayer.IsValid()) return;
	m_EditorScene->PrepareFrame();

	//end frame for all viewports (calls end frame on the scene)
	for (auto& viewport : m_Viewports)
	{
		viewport->EndFrame();
	}

	//render for each active asset editor
	for (uint i = 0; i < m_ActiveAssetEditors.size(); i++)
	{
		m_ActiveAssetEditors[i]->Render();
	}
	
	m_EditorScene->ClearFrame();
	m_ImGuiLayer.End();
	GetWindow()->EndFrame();
}

void EditorApp::HotReload()
{
// 	Buffer sceneBuffer;
// 	m_EditorScene->Serialize()
// 	GetModuleManager()->UnloadAllModules();
}

Ref<Viewport> EditorApp::CreateViewport(Ref<Scene> scene)
{
	Ref<Viewport> viewport = MakeRef<Viewport>(scene, GetWindow()->GetRenderAPI(), nullptr, std::string("Viewport " + STRING(m_Viewports.size())));
	m_Viewports.push_back(viewport);
	return viewport;
}

void EditorApp::DestroyViewport(Ref<Viewport> viewport)
{
	m_Viewports.erase(std::find(m_Viewports.begin(), m_Viewports.end(), viewport));
}


void EditorApp::OnKeyDown(KeyEvent* event)
{

}

void EditorApp::AddAssetEditor(Ref<AssetHandle> TargetAssetHandle)
{
	std::string assetType = TargetAssetHandle->GetAssetType();
	Ref<AssetEditor> NewEditor = nullptr;

	if(assetType == "TextureAsset")
	{
		NewEditor = MakeRef<TextureAssetEditor>();
	}

	if (assetType == "PrefabAsset")
	{
		NewEditor = MakeRef<PrefabAssetEditor>();
	}

	if(!NewEditor) return;
	NewEditor->m_TargetAsset = TargetAssetHandle;
	NewEditor->m_App = this;
	NewEditor->Init();
	m_ActiveAssetEditors.push_back(NewEditor);
}

void EditorApp::RemoveAssetEditor(AssetEditor* assetEditor)
{
	auto remove = m_ActiveAssetEditors.end();
	for (auto it = m_ActiveAssetEditors.begin(); it != m_ActiveAssetEditors.end(); it++)
	{
		if (assetEditor == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_ActiveAssetEditors.end())
	{
		m_ActiveAssetEditors.erase(remove);
	}
}
