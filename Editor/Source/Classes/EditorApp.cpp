#include "EditorApp.h"
#include "Event/Event.h"

EditorApp::EditorApp() : Application()
{
	MakeWindow("DEditor", 1280, 720, false);
	GetWindow()->SetVsync(false);

	//close event for window
	m_WindowEvent.Assign([&](WindowEvent* event)
	{
		if (event->GetEventType() == WindowEventType::CLOSED)
		{
			m_ImGuiLayer.Shutdown();
		}
	});

	m_ModuleEvent.Assign([&](ModuleEvent* event)
	{
		if(event->GetEventType() == ModuleEventType::LOADED)
			LogTemp("LOADED : " + event->m_ModuleName);

		if (event->GetEventType() == ModuleEventType::UNLOADED)
			LogTemp("UNLOADED : " + event->m_ModuleName);
	});

	//bind the window event callback 
	GetEventDispatcher().Bind(m_WindowEvent);

	//bind the module event callback
	GetEventDispatcher().Bind(m_ModuleEvent);

	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());

	m_ImGuiLayer.Init(GetWindow());
}

void EditorApp::OnUpdate(const Tick& tick)
{
	GetWindow()->StartFrame();
	m_ImGuiLayer.Begin();

	if(m_ImGuiLayer.IsValid())
	{ 

		ImGui::Begin("Application Objects");

		for (auto& obj : GetAppObjects())
		{
			if (ImGui::Button(obj->GetClassType().Name.c_str()))
			{
				
			}
		}

		ImGui::End();

		ImGui::Begin("Registry");

		if (ImGui::TreeNode("Objects"))
		{
			for (auto& reg : GetRegistry().GetRegisteredKeys())
			{
				if (reg.type == RegistryType::OBJECT)
				{
					if (ImGui::Button(reg.name.c_str()))
					{
						(GetRegistry().Make(reg))->Initialize(ObjectInitializer());
					}
				}
			}

			ImGui::TreePop();
		}
		if (ImGui::TreeNode("App Objects"))
		{
			for (auto& reg : GetRegistry().GetRegisteredKeys())
			{
				if (reg.type == RegistryType::APPOBJECT)
				{
					if (ImGui::Button(reg.name.c_str()))
					{
						AddAppObject(Cast<AppObject>(GetRegistry().Make(reg)));
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

	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 0.2, 0.3);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	glEnd();

	m_ImGuiLayer.End();
	GetWindow()->EndFrame();
}
