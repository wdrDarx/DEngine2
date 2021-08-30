#include "VRApp.h"

VRApp::VRApp() : Application()
{
	vr::EVRInitError error;
	m_VRsystem = vr::VR_Init(&error, vr::VRApplication_Scene);
	if (error != 0)
		LogTemp("VR Error : " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(error)));

	uint width, height;
	m_VRsystem->GetRecommendedRenderTargetSize(&width, &height);

	MakeWindow("Runtime", width, height, false);
	GetWindow()->GetRenderAPI()->SetViewport({width, height});
	GetWindow()->SetVsync(false);
	GetWindow()->SetInputMode(InputMode::GAME);

	GetAssetManager().DiscoverAssets();
	GetModuleManager().LoadAllModules(Paths::GetModulesDirectory());
	
	m_Camera = MakeRef<Camera>(GetWindow()->GetRenderAPI());
	m_LeftEyeCam = MakeRef<Camera>(GetWindow()->GetRenderAPI());
	m_RightEyeCam = MakeRef<Camera>(GetWindow()->GetRenderAPI());

	FrameBufferSpec spec;
	spec.Width = width;
	spec.Height = height;
	m_LeftEyeBuffer = MakeRef<FrameBuffer>(spec);
	m_RightEyeBuffer = MakeRef<FrameBuffer>(spec);

	m_openScene = CreateAppObject<Scene>();
	m_openScene->SetPipeline<DefaultPipeline>(GetWindow()->GetRenderAPI());

	SetAppState(AppState::GAME);

	Ref<SceneAsset> scene = GetAssetManager().LoadAsset<SceneAsset>(Paths::GetContentDirectory() + "\\Scenes\\VRTest.SceneAsset");
	SceneUtils::LoadSceneFromAsset(scene, m_openScene);
	m_openScene->OnBeginPlay();

	m_LeftController = SceneUtils::SpawnPrefabInScene(AssetRef<PrefabAsset>(Paths::GetContentDirectory() + "Prefabs\\VR_Controller.PrefabAsset"), m_openScene.get(), Transform(), ObjectInitializer());
	m_RightController = SceneUtils::SpawnPrefabInScene(AssetRef<PrefabAsset>(Paths::GetContentDirectory() + "Prefabs\\VR_Controller.PrefabAsset"), m_openScene.get(), Transform(), ObjectInitializer());

	m_VRcompositor = vr::VRCompositor();
	m_VRChaperone = vr::VRChaperone();

	m_LeftEyeProj = Mat4From4VR(m_VRsystem->GetProjectionMatrix(vr::Eye_Left, 0.01f, 10000.f));
	m_RightEyeProj = Mat4From4VR(m_VRsystem->GetProjectionMatrix(vr::Eye_Right, 0.01f, 10000.f));
	m_VRScaleMat = glm::scale(glm::mat4(1.0f), { m_VRScale, m_VRScale, m_VRScale });

	if (!GetWindow()->GetRenderAPI()->IsShaderInCache("BasicShader"))
		GetWindow()->GetRenderAPI()->AddShaderToCache(MakeRef<Shader>(Paths::GetEngineDirectory() + "Shaders\\BasicShader.shader"), "BasicShader");
}

void VRApp::OnUpdate(const Tick& tick)
{
	GetWindow()->StartFrame();
	if(!GetWindow()->isContextBound()) return;

	GetWindow()->GetRenderAPI()->SetViewport({GetWindow()->GetWidth(), GetWindow()->GetHeight()});
	std::string title = "FPS : " + STRING(1.0 / tick.DeltaTime);
	glfwSetWindowTitle(GetWindow()->GetGlfwWindow(), title.c_str());

	GetWindow()->GetInputManager().ForwardTo(m_openScene->GetInputManager());

	if(true)
	{
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
	}

	Super::OnUpdate(tick);

	// for somebody asking for the default figure out the time from now to photons.
	float fSecondsSinceLastVsync;
	m_VRsystem->GetTimeSinceLastVsync(&fSecondsSinceLastVsync, NULL);
	float fDisplayFrequency = m_VRsystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
	float fFrameDuration = 1.f / fDisplayFrequency;
	float fVsyncToPhotons = m_VRsystem->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);
	float fPredictedSecondsFromNow = fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;

	//get headset transform
	vr::TrackedDevicePose_t* PosArray = new vr::TrackedDevicePose_t();
	m_VRsystem->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, fPredictedSecondsFromNow, PosArray, 1);
	glm::mat4 WorldTrans = Mat4From3x4VR(PosArray->mDeviceToAbsoluteTracking);
	glm::mat4 FinalWorldTrans = m_VRScale * WorldTrans;
	m_HeadsetTransform = World::MakeTransform(FinalWorldTrans);
	//m_HeadsetTransform.pos *= m_VRScale;


	//logs
	//LogTemp("Pos : " + Log::string(m_HeadsetTransform.pos));
	//LogTemp("Rot : " + Log::string(trans.rot));


	//controllers
	auto LeftHandIndex = m_VRsystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	auto RightHandIndex = m_VRsystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

	vr::VRControllerState_t LeftControllerState;
	vr::VRControllerState_t RightControllerState;
	vr::TrackedDevicePose_t* LeftControllerPose = new vr::TrackedDevicePose_t();
	vr::TrackedDevicePose_t* RightControllerPose = new vr::TrackedDevicePose_t();

	m_VRsystem->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, LeftHandIndex, &LeftControllerState, sizeof(LeftControllerState), LeftControllerPose);
	m_VRsystem->GetControllerStateWithPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, RightHandIndex, &RightControllerState, sizeof(RightControllerState), RightControllerPose);

	Transform LeftTrans = World::MakeTransform(m_VRScale * Mat4From3x4VR(LeftControllerPose->mDeviceToAbsoluteTracking));
	Transform RightTrans = World::MakeTransform(m_VRScale * Mat4From3x4VR(RightControllerPose->mDeviceToAbsoluteTracking));
	LeftTrans.scale = vec3d(1.0f);
	RightTrans.scale = vec3d(1.0f);
	LogTemp("Left Pos : " + Log::string(LeftTrans.pos));
	if(LeftControllerPose->bPoseIsValid && RightControllerPose->bPoseIsValid)
	{ 
		m_LeftController->GetRootComponent()->SetWorldTransform(LeftTrans);
		m_RightController->GetRootComponent()->SetWorldTransform(RightTrans);
	}

	//left eye
	m_LeftEyeBuffer->Bind();
	m_openScene->GetPipeline()->GetRenderAPI()->SetClearColor({ 0,0,0,1 });
	m_openScene->GetPipeline()->GetRenderAPI()->Clear();

	//eye offset
	glm::mat4 LeftEyeMatrix = Mat4From3x4VR(m_VRsystem->GetEyeToHeadTransform(vr::Eye_Left));	
	m_LeftEyeCam->SetProjectionMatrix(m_LeftEyeProj);
	m_LeftEyeCam->SetViewMatrix(glm::inverse(LeftEyeMatrix) * glm::inverse(World::MakeMatrix(m_HeadsetTransform)));
	m_LeftEyeCam->SetViewProjectionMatrix(m_LeftEyeCam->GetProjectionMatrix() * m_LeftEyeCam->GetViewMatrix());

	m_openScene->GetPipeline()->ClearFrame();
	m_openScene->GetPipeline()->PrepareFrame();
	m_openScene->GetPipeline()->RenderFrame(m_LeftEyeCam);
	m_LeftEyeBuffer->Unbind();

	//right eye
	m_RightEyeBuffer->Bind();
	m_openScene->GetPipeline()->GetRenderAPI()->SetClearColor({ 0,0,0,1 });
	m_openScene->GetPipeline()->GetRenderAPI()->Clear();

	//eye offset
	glm::mat4 RightEyeMatrix = Mat4From3x4VR(m_VRsystem->GetEyeToHeadTransform(vr::Eye_Right));
	m_RightEyeCam->SetProjectionMatrix(m_RightEyeProj);
	m_RightEyeCam->SetViewMatrix(glm::inverse(RightEyeMatrix) * glm::inverse(World::MakeMatrix(m_HeadsetTransform)));
	m_RightEyeCam->SetViewProjectionMatrix(m_RightEyeCam->GetProjectionMatrix() * m_RightEyeCam->GetViewMatrix());

	m_openScene->GetPipeline()->ClearFrame();
	m_openScene->GetPipeline()->PrepareFrame();
	m_openScene->GetPipeline()->RenderFrame(m_RightEyeCam);
	m_RightEyeBuffer->Unbind();

	//render debug left eye
	Ref<Shader> basicShader = GetWindow()->GetRenderAPI()->GetShaderFromCache("BasicShader");
	basicShader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_LeftEyeBuffer->GetColorAttachement());
	basicShader->SetUniform1i("u_Texture", 0);
	RenderUtils::RenderQuad();


	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	m_VRcompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	//submit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_LeftEyeBuffer->GetColorAttachement());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_RightEyeBuffer->GetColorAttachement());

	vr::Texture_t* texture = new vr::Texture_t();
	texture->handle = (void*)(m_LeftEyeBuffer->GetColorAttachement());
	texture->eColorSpace = vr::ColorSpace_Linear;
	texture->eType = vr::TextureType_OpenGL;

	//submit left eye
	m_VRcompositor->Submit(vr::EVREye::Eye_Left, texture);

	//submit right eye
	texture->handle = (void*)(m_RightEyeBuffer->GetColorAttachement());
	m_VRcompositor->Submit(vr::EVREye::Eye_Right, texture);
	m_VRcompositor->PostPresentHandoff();

	GetWindow()->EndFrame();
}

glm::mat4 VRApp::Mat4From4VR(const vr::HmdMatrix44_t& mat)
{
	glm::mat4 out;
	out[0][0] = mat.m[0][0]; out[1][0] = mat.m[0][1]; out[2][0] = mat.m[0][2]; out[3][0] = mat.m[0][3];
	out[0][1] = mat.m[1][0]; out[1][1] = mat.m[1][1]; out[2][1] = mat.m[1][2]; out[3][1] = mat.m[1][3];
	out[0][2] = mat.m[2][0]; out[1][2] = mat.m[2][1]; out[2][2] = mat.m[2][2]; out[3][2] = mat.m[2][3];
	out[0][3] = mat.m[3][0]; out[1][3] = mat.m[3][1]; out[2][3] = mat.m[3][2]; out[3][3] = mat.m[3][3];

	return out;
}

glm::mat4 VRApp::Mat4From3x4VR(const vr::HmdMatrix34_t& mat)
{
	glm::mat4 out;
	out[0][0] = mat.m[0][0]; out[1][0] = mat.m[0][1]; out[2][0] = mat.m[0][2]; out[3][0] = mat.m[0][3];
	out[0][1] = mat.m[1][0]; out[1][1] = mat.m[1][1]; out[2][1] = mat.m[1][2]; out[3][1] = mat.m[1][3];
	out[0][2] = mat.m[2][0]; out[1][2] = mat.m[2][1]; out[2][2] = mat.m[2][2]; out[3][2] = mat.m[2][3];
	out[0][3] = 0;			 out[1][3] = 0;           out[2][3] = 0;           out[3][3] = 1; 

	return out;
}

