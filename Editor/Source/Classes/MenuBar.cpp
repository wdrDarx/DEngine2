#include "MenuBar.h"
#include "EditorApp.h"

void MenuBar::Render(EditorApp* m_App)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Viewport"))
		{
			if (ImGui::MenuItem("Create New Viewport"))
			{
				m_App->CreateViewport(m_App->m_EditorScene);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
