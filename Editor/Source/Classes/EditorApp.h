#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"
#include "PropertyWindow.h"
#include "Viewport.h"
#include "MenuBar.h"
#include "SceneObjectPannel.h"

class EditorApp : public Application
{
public:
	EditorApp();

	void OnUpdate(const Tick& tick) override;
	void RenderImGui(const Tick& tick);
	void BeginFrame();
	void EndFrame();

	void HotReload();

	void CreateViewport(Ref<Scene> scene);

	std::vector<Ref<Viewport>> m_Viewports;
	Ref<Scene> m_EditorScene;
	Callback<WindowEvent> m_WindowEvent;
	Callback<ModuleEvent> m_ModuleEvent;
	Callback<SceneEvent> m_SceneEvent;
	Callback<ApplicationEvent> m_ApplicationEvent;
	ImGuiLayer m_ImGuiLayer;
	PropertyWindow m_PropertyWindow;
	MenuBar m_MenuBar;
	SceneObjectPannel m_SceneObjectPannel;
};

