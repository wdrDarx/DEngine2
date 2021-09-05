#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"
#include "PropertyWindow.h"
#include "Viewport.h"
#include "MenuBar.h"
#include "SceneObjectPannel.h"
#include "ContentBrowser.h"
#include "AssetEditor.h"
#include "ImGuizmo.h"

class EditorApp : public Application
{
public:
	EditorApp();
	using Super = Application;

	void OnUpdate(const Tick& tick) override;
	void RenderImGui(const Tick& tick);
	void BeginFrame();
	void EndFrame();

	void DrawRendererWindow();

	void HotReload();
	
	void BeginPlay();
	void EndPlay();

	void UnloadAll();
	void LoadAll();

	Ref<Viewport> CreateViewport(Ref<Scene> scene);
	void DestroyViewport(Viewport* viewport);

	void OnKeyDown(KeyEvent* event);
	void OnFileDropped(const std::string& FullPath);

	void AddAssetEditor(Ref<AssetHandle> TargetAssetHandle);
	void RemoveAssetEditor(AssetEditor* assetEditor);

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
	ContentBrowser m_ContentBrowser;

	Ref<SceneAsset> m_HotReloadSceneAsset;
	Ref<SceneAsset> m_BeginPlaySceneAsset;

	Ref<PhysicsWorld> m_PhysicsWorld;
	Ref<DropTarget> m_DropTarget;

	RenderFlags m_DebugFlags = (RenderFlags)0;

	Callback<KeyEvent> m_OnKeyDownCallback;
	ImGuizmo::OPERATION m_TransformMode = ImGuizmo::TRANSLATE;
	std::vector<Ref<AssetEditor>> m_ActiveAssetEditors;
};

