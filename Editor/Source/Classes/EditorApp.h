#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"
#include "PropertyWindow.h"

class EditorApp : public Application
{
public:
	EditorApp();

	void OnUpdate(const Tick& tick) override;

	Ref<Scene> m_EditorScene;
	Callback<WindowEvent> m_WindowEvent;
	Callback<ModuleEvent> m_ModuleEvent;
	Callback<SceneEvent> m_SceneEvent;
	ImGuiLayer m_ImGuiLayer;
	PropertyWindow m_PropertyWindow;
};

