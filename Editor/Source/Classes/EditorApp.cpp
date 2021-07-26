#include "EditorApp.h"
#include "Event/Event.h"

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

		if (event->GetEventType() == WindowEventType::RESIZED)
		{
			//LogTemp("New Window Size : " + Log::string(event->GetNewSize()));
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

	//Scene events
	m_SceneEvent.Assign([&](SceneEvent* event)
	{
		//Auto unassign selected scene object in property window if that object was deleted
		if (event->GetEventType() == SceneEventType::OBJECT_PRE_DELETE)
		{
			if (event->GetSceneObject() && m_PropertyWindow.m_SelectedSceneObject.get() == event->GetSceneObject())
			{
				m_PropertyWindow.m_SelectedSceneObject = nullptr;
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

	//init imgui layer
	m_ImGuiLayer.Init(GetWindow());

	//Make the scene
	m_EditorScene = CreateAppObject<Scene>();

	//bind scene events
	m_EditorScene->GetSceneEventDipatcher().Bind(m_SceneEvent);

	//property window
	m_PropertyWindow.Init(ToRef<EditorApp>(this));

	//main viewport
	CreateViewport(m_EditorScene);
}

void EditorApp::OnUpdate(const Tick& tick)
{
	BeginFrame();

	if (m_ImGuiLayer.IsValid())
	{
		m_MenuBar.Render(this);
		m_PropertyWindow.Render();
		ImGui::Begin("Application Objects");

		for (auto& obj : GetAppObjects())
		{
			if (ImGui::Button(obj->GetClassType().Name.c_str()))
			{

			}
		}

		ImGui::End();

		ImGui::Begin("Scene Objects");

		for (auto& obj : m_EditorScene->GetSceneObjects())
		{
			if (ImGui::Button((obj->GetClassType().Name + " " + STRING(obj->GetID().ID)).c_str()))
			{
				m_PropertyWindow.m_SelectedSceneObject = obj;
			}
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
}

void EditorApp::BeginFrame()
{
	GetWindow()->StartFrame();
	m_ImGuiLayer.Begin();
	if (!m_ImGuiLayer.IsValid()) return;

	//begin frame for all viewports (requires imGui context)
	for (auto& viewport : m_Viewports)
	{
		viewport->BeginFrame();
	}
}

void EditorApp::EndFrame()
{
	if (!m_ImGuiLayer.IsValid()) return;

	//end frame for all viewports (calls end frame on the scene)
	for (auto& viewport : m_Viewports)
	{
		viewport->EndFrame();
	}
	
	m_EditorScene->ClearFrame();
	m_ImGuiLayer.End();
	GetWindow()->EndFrame();
}

void EditorApp::CreateViewport(Ref<Scene> scene)
{
	m_Viewports.push_back(MakeRef<Viewport>(scene, GetWindow()->GetRenderAPI(), nullptr, std::string("Viewport " + STRING(m_Viewports.size()))));
}
