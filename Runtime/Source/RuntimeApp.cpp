#include "RuntimeApp.h"

RuntimeApp::RuntimeApp() : Application()
{
	MakeWindow("Runtime", 1920, 1080, false);
	GetWindow()->SetVsync(false);
	GetWindow()->SetInputMode(InputMode::GAME);
	GetAssetManager().DiscoverAssets();
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());

	
	m_Camera = MakeRef<Camera>(GetWindow()->GetRenderAPI());
	m_openScene = CreateAppObject<Scene>();
	m_openScene->SetPipeline<DefaultPipeline>(GetWindow()->GetRenderAPI());
}

void RuntimeApp::OnUpdate(const Tick& tick)
{
	GetWindow()->StartFrame();
	GetWindow()->GetRenderAPI()->SetViewport({GetWindow()->GetWidth(), GetWindow()->GetHeight()});
	std::string title = "FPS : " + STRING(1.0 / tick.DeltaTime);
	glfwSetWindowTitle(GetWindow()->GetGlfwWindow(), title.c_str());
	GetWindow()->GetInputManager().ForwardTo(m_openScene->GetInputManager());
	auto trans = m_Camera->GetTransform();
	float m_CameraRotationSpeed = 100.f;
	float m_CameraMovementSpeed = 100.f;
	//movement
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_W))
	{
		trans.pos += World::GetForwardVector(trans.rot) * m_CameraMovementSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_S))
	{
		trans.pos -= World::GetForwardVector(trans.rot) * m_CameraMovementSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_A))
	{
		trans.pos -= World::GetRightVector(trans.rot) * m_CameraMovementSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_D))
	{
		trans.pos += World::GetRightVector(trans.rot) * m_CameraMovementSpeed * tick.DeltaTime;
	}

	//Rotation 
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_UP))
	{
		trans.rot.x += m_CameraRotationSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_DOWN))
	{
		trans.rot.x -= m_CameraRotationSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_LEFT))
	{
		trans.rot.y += m_CameraRotationSpeed * tick.DeltaTime;
	}
	if (GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_RIGHT))
	{
		trans.rot.y -= m_CameraRotationSpeed * tick.DeltaTime;
	}
	m_Camera->SetTransform(trans);
	Super::OnUpdate(tick);
	
	m_openScene->GetPipeline()->ClearFrame();

	m_openScene->GetPipeline()->PrepareFrame();
	
	m_openScene->GetPipeline()->RenderFrame(m_Camera);


	GetWindow()->EndFrame();

	if (!loaded)
	{	
		Ref<SceneAsset> scene = GetAssetManager().LoadAsset<SceneAsset>(Paths::GetContentDirectory() + "\\Scenes\\BuildingTest.SceneAsset");
		SceneUtils::LoadSceneFromAsset(scene, m_openScene);
		loaded = true;
	}
	
}
