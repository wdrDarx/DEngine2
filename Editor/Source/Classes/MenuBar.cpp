#include "MenuBar.h"
#include "EditorApp.h"

void MenuBar::Init()
{
	//folder icon
	std::string PlayIconPath = Paths::GetEngineDirectory() + "Resources\\PlayButton.png";
	if (File::DoesFileExist(PlayIconPath))
		m_PlayIcon = MakeRef<Texture>(Image(PlayIconPath));
	else
		m_PlayIcon = MakeRef<Texture>(TextureSpec());

	//file icon
	std::string StopPlayPath = Paths::GetEngineDirectory() + "Resources\\StopButton.png";
	if (File::DoesFileExist(StopPlayPath))
		m_StopIcon = MakeRef<Texture>(Image(StopPlayPath));
	else
		m_StopIcon = MakeRef<Texture>(TextureSpec());
}

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

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.f);
		if (m_App->GetAppState() == AppState::GAME)
		{
			if (ImGui::ImageButton((ImTextureID)m_StopIcon->GetBinding(), { m_IconSize,m_IconSize }, { 0,1 }, { 1,0 }))
			{
				m_App->EndPlay();		
			}
		}
		else
		{
			if (ImGui::ImageButton((ImTextureID)m_PlayIcon->GetBinding(), { m_IconSize,m_IconSize }, { 0,1 }, { 1,0 }))
			{
				m_App->BeginPlay();
			}
		}

		ImGui::EndMainMenuBar();
	}
}
