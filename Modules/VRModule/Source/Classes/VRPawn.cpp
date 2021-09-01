#include "VRPawn.h"

void VRPawn::OnConstruct()
{
	Super::OnConstruct();

	root = CreateComponent<TransformComponent>(ObjectInitializer::Module(this), "Root");
	HMD = CreateComponent<CameraComponent>(ObjectInitializer::Module(this), "HMD");

	SetRootComponent(root);
	HMD->AttachTo(root);
}

void VRPawn::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(tick.GetTickGroup() != TickGroup::GAME) return;

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
	glm::mat4 WorldTrans = VRModule::Mat4From3x4VR(PosArray->mDeviceToAbsoluteTracking);
	glm::mat4 FinalWorldTrans = m_VRScaleMat * WorldTrans;
	HMD->SetLocalTransform(World::MakeTransform(FinalWorldTrans));

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

	glm::mat4 LeftLocal = m_VRScaleMat * VRModule::Mat4From3x4VR(LeftControllerPose->mDeviceToAbsoluteTracking);
	glm::mat4 RightLocal = m_VRScaleMat * VRModule::Mat4From3x4VR(RightControllerPose->mDeviceToAbsoluteTracking);

	Transform LeftTrans = World::MakeTransform( root->GetWorldMatrix() * LeftLocal);
	Transform RightTrans = World::MakeTransform( root->GetWorldMatrix() * RightLocal);
	LeftTrans.scale = vec3d(1.0f);
	RightTrans.scale = vec3d(1.0f);

	if (LeftControllerPose->bPoseIsValid && RightControllerPose->bPoseIsValid)
	{
		m_LeftController->GetRootComponent()->SetWorldTransform(LeftTrans);
		m_RightController->GetRootComponent()->SetWorldTransform(RightTrans);
	}

	//set camera to the right eye
	GetScene()->SetActiveCamera(m_RightEyeCam);
}

void VRPawn::OnBeginPlay()
{
	Super::OnBeginPlay();

	vr::EVRInitError error;
	m_VRsystem = vr::VR_Init(&error, vr::VRApplication_Scene);
	if (error != 0)
		LogTemp("VR Error : " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(error)));

	uint width, height;
	m_VRsystem->GetRecommendedRenderTargetSize(&width, &height);

	m_LeftController = SceneUtils::SpawnPrefabInScene(ControllerPrefab, GetScene(), Transform(), ObjectInitializer::Module(this));
	m_RightController = SceneUtils::SpawnPrefabInScene(ControllerPrefab, GetScene(), Transform(), ObjectInitializer::Module(this));

	m_LeftEyeCam = MakeRef<Camera>(GetScene()->GetPipeline()->GetRenderAPI());
	m_RightEyeCam = MakeRef<Camera>(GetScene()->GetPipeline()->GetRenderAPI());

	FrameBufferSpec spec;
	spec.Width = width;
	spec.Height = height;
	m_LeftEyeBuffer = MakeRef<FrameBuffer>(spec);
	m_RightEyeBuffer = MakeRef<FrameBuffer>(spec);

	m_VRcompositor = vr::VRCompositor();
	m_VRChaperone = vr::VRChaperone();

	m_LeftEyeProj = VRModule::Mat4From4VR(m_VRsystem->GetProjectionMatrix(vr::Eye_Left, 0.01f, 10000.f));
	m_RightEyeProj = VRModule::Mat4From4VR(m_VRsystem->GetProjectionMatrix(vr::Eye_Right, 0.01f, 10000.f));
	m_VRScaleMat = glm::scale(glm::mat4(1.0f), { m_VRScale, m_VRScale, m_VRScale });

	//assign the render frame function to be called after all objects are updated
	m_ApplicationCallback.Assign([&](ApplicationEvent* event)
	{
		RenderFrame();	
	});

	GetScene()->GetApplication()->GetEventDispatcher().Bind(m_ApplicationCallback);
}

void VRPawn::OnEndPlay()
{
	Super::OnEndPlay();

	vr::VR_Shutdown();
}

void VRPawn::RenderFrame()
{
	vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	m_VRcompositor->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);

	//left eye
	m_LeftEyeBuffer->Bind();
	GetScene()->GetPipeline()->GetRenderAPI()->SetClearColor({ 0,0,0,0 });
	GetScene()->GetPipeline()->GetRenderAPI()->Clear();
	GetScene()->GetPipeline()->PrepareFrame();

	//eye offset
	glm::mat4 LeftEyeMatrix = VRModule::Mat4From3x4VR(m_VRsystem->GetEyeToHeadTransform(vr::Eye_Left));
	m_LeftEyeCam->SetProjectionMatrix(m_LeftEyeProj);
	m_LeftEyeCam->SetViewMatrix(glm::inverse(LeftEyeMatrix) * glm::inverse(World::MakeMatrix(HMD->GetWorldTransform())));
	m_LeftEyeCam->SetViewProjectionMatrix(m_LeftEyeCam->GetProjectionMatrix() * m_LeftEyeCam->GetViewMatrix());

	GetScene()->GetPipeline()->RenderFrame(m_LeftEyeCam);
	m_LeftEyeBuffer->Unbind();

	//right eye
	m_RightEyeBuffer->Bind();
	GetScene()->GetPipeline()->GetRenderAPI()->SetClearColor({ 0,0,0,0 });
	GetScene()->GetPipeline()->GetRenderAPI()->Clear();

	//eye offset
	glm::mat4 RightEyeMatrix = VRModule::Mat4From3x4VR(m_VRsystem->GetEyeToHeadTransform(vr::Eye_Right));
	m_RightEyeCam->SetProjectionMatrix(m_RightEyeProj);
	m_RightEyeCam->SetViewMatrix(glm::inverse(RightEyeMatrix) * glm::inverse(World::MakeMatrix(HMD->GetWorldTransform())));
	m_RightEyeCam->SetViewProjectionMatrix(m_RightEyeCam->GetProjectionMatrix() * m_RightEyeCam->GetViewMatrix());

	GetScene()->GetPipeline()->RenderFrame(m_RightEyeCam);
	m_RightEyeBuffer->Unbind();

	//submit
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_LeftEyeBuffer->GetColorAttachement());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_RightEyeBuffer->GetColorAttachement());

	vr::Texture_t* texture = new vr::Texture_t();
	texture->handle = (void*)(m_LeftEyeBuffer->GetColorAttachement());
	texture->eColorSpace = vr::ColorSpace_Gamma;
	texture->eType = vr::TextureType_OpenGL;

	//submit left eye
	m_VRcompositor->Submit(vr::EVREye::Eye_Left, texture);

	//submit right eye
	texture->handle = (void*)(m_RightEyeBuffer->GetColorAttachement());
	m_VRcompositor->Submit(vr::EVREye::Eye_Right, texture);
	m_VRcompositor->PostPresentHandoff();
}
