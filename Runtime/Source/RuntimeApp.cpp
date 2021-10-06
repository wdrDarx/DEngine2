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

	Ref<SceneAsset> scene = GetAssetManager().LoadAsset<SceneAsset>(Paths::GetContentDirectory() + "\\Runtime.SceneAsset");
	m_openScene->OnBeginPlay();
	SceneUtils::LoadSceneFromAsset(scene, m_openScene);	

	FrameBufferSpec spec;
	spec.Samples = 1;
	spec.Width = 1920;
	spec.Height = 1080;
	m_ScreenFrameBuffer = MakeRef<FrameBuffer>(spec);
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
	m_openScene->GetPipeline()->GetRenderer<MeshRenderer>()->GetSettingsMutable().OcclusionCulling = true;
	
	GetWindow()->GetRenderAPI()->SetViewport({ GetWindow()->GetWidth(), GetWindow()->GetHeight() });

	m_ScreenFrameBuffer->Bind();
	m_openScene->GetPipeline()->GetRenderAPI()->SetClearColor({ 0,0,0,0 });
	m_openScene->GetPipeline()->GetRenderAPI()->Clear();
	m_openScene->GetPipeline()->RenderFrame(m_openScene->GetActiveCamera());
	m_ScreenFrameBuffer->Unbind();

	auto shader = m_openScene->GetPipeline()->GetRenderAPI()->GetShaderFromCache("BasicShader");
	shader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ScreenFrameBuffer->GetColorAttachement());

	shader->SetUniform1i("u_Texture", 0);
	RenderUtils::RenderQuad();

	m_openScene->GetPipeline()->ClearFrame();

	


	GetWindow()->EndFrame();
}
