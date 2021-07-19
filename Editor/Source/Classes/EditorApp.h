#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"

class EditorApp : public Application
{
public:
	EditorApp();

	void OnUpdate(const Tick& tick) override;


	Callback<WindowEvent> m_WindowEvent;
	Callback<ModuleEvent> m_ModuleEvent;
	ImGuiLayer m_ImGuiLayer;
};

