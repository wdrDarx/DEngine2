#include "EditorApp.h"
#include "Event/Event.h"

EditorApp::EditorApp() : Application()
{
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());

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

	//bind the window event callback 
	GetEventDispatcher().Bind(m_WindowEvent);

	m_ImGuiLayer.Init(GetWindow());
}

void EditorApp::OnUpdate(const Tick& tick)
{
	GetWindow()->StartFrame();
	m_ImGuiLayer.Begin();

	if(m_ImGuiLayer.IsValid())
	{ 
		ImGui::Begin("Registry");

		for (auto& reg : GetRegistry().GetRegisteredKeys())
		{	
			if (ImGui::Button(reg.name.c_str()))
			{
				(GetRegistry().Make(reg))->Initialize(ObjectInitializer());
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
