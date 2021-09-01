#include "RuntimeApp.h"

RuntimeApp::RuntimeApp() : Application()
{
	MakeWindow("Runtime", 1920, 1080, false);
	GetWindow()->SetVsync(false);
	GetWindow()->SetInputMode(InputMode::GAME);
	GetAssetManager().DiscoverAssets();
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());

	m_openScene = CreateAppObject<Scene>();
	CreateAppObject<PhysicsWorld>();
	m_openScene->SetPipeline<DefaultPipeline>(GetWindow()->GetRenderAPI());

	SetAppState(AppState::GAME);
	

	Ref<SceneAsset> scene = GetAssetManager().LoadAsset<SceneAsset>(Paths::GetContentDirectory() + "\\Scenes\\VRTest.SceneAsset");
	SceneUtils::LoadSceneFromAsset(scene, m_openScene);
	m_openScene->OnBeginPlay();
}

void RuntimeApp::OnUpdate(const Tick& tick)
{
	SetAppType(AppType::SHIPPING);
	GetWindow()->StartFrame();
	if(!GetWindow()->isContextBound()) return;

	GetWindow()->GetRenderAPI()->SetViewport({GetWindow()->GetWidth(), GetWindow()->GetHeight()});
	std::string title = "FPS : " + STRING(1.0 / tick.DeltaTime);
	glfwSetWindowTitle(GetWindow()->GetGlfwWindow(), title.c_str());
	GetWindow()->GetInputManager().ForwardTo(m_openScene->GetInputManager());
	
	Super::OnUpdate(tick);
	
	m_openScene->GetPipeline()->PrepareFrame();
	
	m_openScene->GetPipeline()->RenderFrame(m_openScene->GetActiveCamera());

	m_openScene->GetPipeline()->ClearFrame();

	GetWindow()->EndFrame();
}
