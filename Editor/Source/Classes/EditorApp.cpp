#include "EditorApp.h"
#include "Event/Event.h"

//asset editors
#include "Classes/AssetEditors/TextureAssetEditor.h"
#include "Classes/AssetEditors/PrefabAssetEditor.h"
#include "Classes/AssetEditors/SceneAssetEditor.h"
#include "Classes/AssetEditors/ObjectAssetEditor.h"
#include "Classes/AssetEditors/MeshAssetEditor.h"

EditorApp::EditorApp() : Application()
{	
	//set app state to editor
	SetAppState(AppState::EDITOR);

	//dev mode
	SetAppType(AppType::DEVELOPMENT);

	MakeWindow("DEditor", 1280, 720, false);
	GetWindow()->SetVsync(false);

	//register drop target 
	m_DropTarget = MakeRef<DropTarget>();
	m_DropTarget->m_Callback = std::bind(&EditorApp::OnFileDropped, this, std::placeholders::_1);
	File::AcceptDragFiles(GetWindow().get(), m_DropTarget.get());

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
			if (event->GetSceneObject() && m_SceneObjectPannel.m_SelectedObject == event->GetSceneObject() || m_PropertyWindow.m_SelectedSceneObject == event->GetSceneObject())
			{
				m_SceneObjectPannel.m_SelectedObject = nullptr;
				m_PropertyWindow.m_SelectedSceneObject = nullptr;
				m_SceneObjectPannel.m_SelectedComponent = nullptr;
				m_PropertyWindow.m_SelectedComponent = nullptr;
			}
		} 

		if (event->GetEventType() == SceneEventType::COMPONENT_PRE_DELETE)
		{
			if (m_SceneObjectPannel.m_SelectedComponent == event->GetComponent() || m_PropertyWindow.m_SelectedComponent == event->GetComponent())
			{
				m_SceneObjectPannel.m_SelectedComponent = nullptr;
				m_PropertyWindow.m_SelectedComponent = nullptr;
				m_SceneObjectPannel.m_SelectedObject = nullptr;
				m_PropertyWindow.m_SelectedSceneObject = nullptr;
			}
		}
	});

	//create the physics world
	m_PhysicsWorld = CreateAppObject<PhysicsWorld>();

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

	//create default pipeline
	m_EditorScene->SetPipeline<DefaultPipeline>(GetWindow()->GetRenderAPI());

	//bind scene events
	m_EditorScene->GetSceneEventDipatcher().Bind(m_SceneEvent);

	//property window
	m_PropertyWindow.Init(this);

	//content browser
	m_ContentBrowser.Init();

	//menu bar
	m_MenuBar.Init();

	//main viewport
	CreateViewport(m_EditorScene);

	//bind key event to input manager
	m_OnKeyDownCallback.Assign(std::bind(&EditorApp::OnKeyDown, this, std::placeholders::_1));
	GetWindow()->GetInputManager().BindOnKeyDown(m_OnKeyDownCallback);

	//create the hot reload scene asset
	m_HotReloadSceneAsset = MakeRef<SceneAsset>();

	//create the begin play scene asset
	m_BeginPlaySceneAsset = MakeRef<SceneAsset>();

	//set debug render flag by default
	auto renderer = m_EditorScene->GetPipeline()->GetRenderer<DebugRenderer>();
	if(renderer)
		renderer->SetRenderFlags(RenderFlags::DEBUGLINES | RenderFlags::DEBUGCUBES);
}

void EditorApp::OnUpdate(const Tick& tick)
{
	//call begin frame before updating any app objects
	BeginFrame();
	Super::OnUpdate(tick);

	//call the rest of the rendering stuff after all objects have updated
	if (m_ImGuiLayer.IsValid())
	{
		RenderImGui(tick);
	}

	//call endframe at the end of the update
	EndFrame();
}

void EditorApp::RenderImGui(const Tick& tick)
{
	m_MenuBar.Render(this);
	m_ContentBrowser.Render(this);

	m_SceneObjectPannel.Render(m_EditorScene.get());
	{
		m_PropertyWindow.m_SelectedSceneObject = m_SceneObjectPannel.m_SelectedObject;
		m_PropertyWindow.m_SelectedComponent = m_SceneObjectPannel.m_SelectedComponent;
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
	if (ImGui::Button("Unload All"))
	{
		GetAssetManager().UnloadAll();
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

	if (ImGui::Button("Unload All"))
	{
		UnloadAll();
	}
	if (ImGui::Button("Load All"))
	{
		LoadAll();
	}

	//set to unload module name (because cant remove elements while in a loop)
	std::string ToUnloadName;
	for (const auto& mod : GetModuleManager().GetLoadedModules())
	{
		ImGui::PushID(mod->m_Name.c_str());
		ImGui::Text(mod->m_Name.c_str());
		ImGui::SameLine();
		if (ImGui::Button("Unload"))
		{
			ToUnloadName = mod->m_Name;
		}
		ImGui::PopID();
	}

	//unload module
	if(!ToUnloadName.empty())
		GetModuleManager().UnloadModule(ToUnloadName);

	ImGui::End();

	DrawRendererWindow();
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

		//set transform mode
		viewport->m_TransformMode = m_TransformMode;

		//begin frame for viewport
		viewport->BeginFrame();		
	}
}

void EditorApp::EndFrame()
{
	if (!m_ImGuiLayer.IsValid()) return;
	m_EditorScene->GetPipeline()->PrepareFrame();

	//end frame for all viewports (calls end frame on the scene)
	for (auto& viewport : m_Viewports)
	{
		viewport->m_SelectedComponent = m_SceneObjectPannel.m_SelectedComponent;
		viewport->m_SelectedObject = m_SceneObjectPannel.m_SelectedObject;
		viewport->EndFrame();
	}

	//remove viewports that were closed
	Viewport* destroy = nullptr;
	for (auto& viewport : m_Viewports)
	{
		if(viewport->m_Close)
			destroy = viewport.get();
	}
	if(destroy)
		DestroyViewport(destroy);

	//render for each active asset editor
	for (uint i = 0; i < m_ActiveAssetEditors.size(); i++)
	{
		m_ActiveAssetEditors[i]->Render();
	}
	
	m_EditorScene->GetPipeline()->ClearFrame();
	m_ImGuiLayer.End();
	GetWindow()->EndFrame();
}

void EditorApp::DrawRendererWindow()
{
	ImGui::Begin("Renderer");
	ImGui::Text(std::string("FPS : " + STRING(int(1.0f / m_LastTick.DeltaTime))).c_str());
	ImGui::Text(std::string("Draw Calls : " + STRING(m_EditorScene->GetPipeline()->GetRenderAPI()->GetRenderStats().DrawCalls)).c_str());
	m_EditorScene->GetPipeline()->GetRenderAPI()->ResetRenderStats();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2,2 });
	bool bDebugRenderer = ImGui::TreeNodeEx((void*)("Debug Renderer"), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen, "Debug Renderer");
	ImGui::PopStyleVar();

	auto Debugrenderer = m_EditorScene->GetPipeline()->GetRenderer<DebugRenderer>();
	if (bDebugRenderer && Debugrenderer)
	{
		bool debuglines = Debugrenderer->GetRenderFlags() & RenderFlags::DEBUGLINES;
		bool debugCubes = Debugrenderer->GetRenderFlags() & RenderFlags::DEBUGCUBES;
		bool physx = Debugrenderer->GetRenderFlags() & RenderFlags::PHYSX;
		ImGui::Checkbox("Debug Lines ", &debuglines);
		ImGui::Checkbox("Debug Cubes ", &debugCubes);
		ImGui::Checkbox("Colliders ", &physx);
		m_DebugFlags = RenderFlags(0);
		if (debuglines)
			m_DebugFlags |= RenderFlags::DEBUGLINES;
		if (debugCubes)
			m_DebugFlags |= RenderFlags::DEBUGCUBES;
		if (physx)
			m_DebugFlags |= RenderFlags::PHYSX;

		Debugrenderer->SetRenderFlags(m_DebugFlags);

		ImGui::DragFloat("Line Width", &Debugrenderer->m_LineWidth, 1.0f, 10.f);
		ImGui::TreePop();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2,2 });
	bool bMeshRenderer = ImGui::TreeNodeEx((void*)("Mesh Renderer"), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen, "Mesh Renderer");
	ImGui::PopStyleVar();

	auto Meshrenderer = m_EditorScene->GetPipeline()->GetRenderer<MeshRenderer>();
	if (bMeshRenderer && Meshrenderer)
	{
		ImGui::DragFloat("Ambient Light Multiplier", &Meshrenderer->GetSettingsMutable().AmbientLightMultiplier, 0.0f, 1.f);
		ImGui::TreePop();
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2,2 });
	bool shaderCache = ImGui::TreeNodeEx((void*)("shadercache"), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen, "Shader Cache");
	ImGui::PopStyleVar();

	if (shaderCache)
	{
		for (const auto& shader : GetWindow()->GetRenderAPI()->GetAllShadersInCache())
		{
			ImGui::PushID(shader.first.c_str());
			ImGui::Columns(2);
			ImGui::Text(shader.first.c_str());
			ImGui::NextColumn();

			if (ImGui::Button("Reload"))
			{
				GetWindow()->GetRenderAPI()->ReloadShader(shader.first);
				LogTemp("Reloaded Shader : " + shader.first);
			}
			ImGui::Columns(1);
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	ImGui::End();
}

void EditorApp::HotReload()
{


}

void EditorApp::BeginPlay()
{
	if (m_EditorScene)
	{
		SetAppState(AppState::GAME);
		m_BeginPlaySceneAsset->SaveScene(m_EditorScene);
		m_EditorScene->OnBeginPlay();
	}
}

void EditorApp::EndPlay()
{
	if (m_EditorScene)
	{
		m_EditorScene->OnEndPlay();
		SetAppState(AppState::EDITOR);

		m_SceneObjectPannel.m_SelectedObject = nullptr;
		m_PropertyWindow.m_SelectedSceneObject = nullptr;
		m_SceneObjectPannel.m_SelectedComponent = nullptr;
		m_PropertyWindow.m_SelectedComponent = nullptr;

		SceneUtils::LoadSceneFromAsset(m_BeginPlaySceneAsset, m_EditorScene);
	}
}

void EditorApp::UnloadAll()
{
	m_HotReloadSceneAsset->SaveScene(m_EditorScene);
	GetModuleManager().UnloadAllModules();
}

void EditorApp::LoadAll()
{
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());
	SceneUtils::LoadSceneFromAsset(m_HotReloadSceneAsset, m_EditorScene);
}

Ref<Viewport> EditorApp::CreateViewport(Ref<Scene> scene)
{
	Ref<Viewport> viewport = MakeRef<Viewport>(scene, GetWindow(), std::string("Viewport " + STRING(m_Viewports.size())));
	m_Viewports.push_back(viewport);
	return viewport;
}

void EditorApp::DestroyViewport(Viewport* viewport)
{
	auto remove = m_Viewports.end();
	for (auto it = m_Viewports.begin(); it != m_Viewports.end(); it++)
	{
		if (viewport == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_Viewports.end())
		m_Viewports.erase(remove);
}


void EditorApp::OnKeyDown(KeyEvent* event)
{
	//gizmo
	if (event->GetKeyCode() == GLFW_KEY_1)
	{
		m_TransformMode = ImGuizmo::TRANSLATE;
	}
	if (event->GetKeyCode() == GLFW_KEY_2)
	{
		m_TransformMode = ImGuizmo::ROTATE;
	}
	if (event->GetKeyCode() == GLFW_KEY_3)
	{
		m_TransformMode = ImGuizmo::SCALE;
	}

	if (event->GetKeyCode() == GLFW_KEY_LEFT_ALT)
	{
		if (m_SceneObjectPannel.m_SelectedObject)
		{
			m_SceneObjectPannel.m_SelectedObject = SceneUtils::CloneSceneObject(m_SceneObjectPannel.m_SelectedObject, m_EditorScene).get();
		}
	}
}

void EditorApp::OnFileDropped(const std::string& FullPath)
{
	auto Save = [&](Ref<Asset> NewAsset, const std::string& saveExtension)
	{
		std::string FinalPath = m_ContentBrowser.m_CurrentPath.string();
		FinalPath +=  "\\" + File::GetFileNameFromPath(FullPath) + "." + saveExtension;
		GetAssetManager().SaveAsset(NewAsset, FinalPath);
	};

	std::string extension = File::GetFileExtenstionFromPath(FullPath);
	if (extension == "fbx" || extension == "FBX")
	{
		Ref<MeshAsset> NewAsset = MakeRef<MeshAsset>();	
		Ref<Mesh> TempMesh = MakeRef<Mesh>();
		ModelLoader::LoadFBX(FullPath, TempMesh.get());
		NewAsset->m_Verticies = TempMesh->GetVerticies();
		NewAsset->m_Indicies = TempMesh->GetIndicies();
		Save(NewAsset, GetAssetManager().GetAssetExtenstion<MeshAsset>());
	}
	if (extension == "png" || extension == "jpg" || extension == "hdr" || extension == "HDR")
	{
		Ref<TextureAsset> NewAsset = MakeRef<TextureAsset>(Image(FullPath));
		Save(NewAsset, GetAssetManager().GetAssetExtenstion<TextureAsset>());
	}
}

void EditorApp::AddAssetEditor(Ref<AssetHandle> TargetAssetHandle)
{
	std::string assetType = TargetAssetHandle->GetAssetType();
	Ref<AssetEditor> NewEditor = nullptr;

	if(assetType == "TextureAsset")
	{
		NewEditor = MakeRef<TextureAssetEditor>();
	} 
	else
	if (assetType == "PrefabAsset")
	{
		NewEditor = MakeRef<PrefabAssetEditor>();
	} 
	else
	if (assetType == "SceneAsset")
	{
		NewEditor = MakeRef<SceneAssetEditor>();
	}
	else
	if (assetType == "MeshAsset")
	{
		NewEditor = MakeRef<MeshAssetEditor>();
	}

	//test if its an ObjectAsset
	Ref<Asset> testAsset = ToRef<Asset>(GetAssetManager().GetAssetTypeRegistry().Make({assetType}));
	if (Cast<ObjectAsset>(testAsset))
	{
		NewEditor = MakeRef<ObjectAssetEditor>();
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
