#pragma once
#include "DEngine.h"
#include "Classes/CameraComponent.h"
#include "Classes/StaticMeshComponent.h"
#include "VRModule.h"

class VRMODULE VRPawn : public SceneObject
{
public:	
	OBJECT_CLASS_DEF(VRPawn, SceneObject)
	OBJECT_PROPS_BEGIN()
		PROPDEF(MovementSpeed, EditAnywhere);
		PROPDEF(ControllerPrefab, EditAnywhere);
	OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnUpdate(const Tick& tick) override;
	void OnBeginPlay() override;
	void OnEndPlay() override;

	//called after all objects have been updated
	void RenderFrame();

public:
	float MovementSpeed = 100.f;
	const float m_VRScale = 100.f;
	glm::mat4 m_VRScaleMat;

	Ref<TransformComponent> root;
	Ref<CameraComponent> HMD;

	Ref<Camera> m_LeftEyeCam;
	Ref<Camera> m_RightEyeCam;
	Ref<FrameBuffer> m_LeftEyeBuffer;
	Ref<FrameBuffer> m_RightEyeBuffer;

	Ref<SceneObject> m_LeftController;
	Ref<SceneObject> m_RightController;
	
	AssetRef<PrefabAsset> ControllerPrefab;

	glm::mat4 m_LeftEyeProj;
	glm::mat4 m_RightEyeProj;

	vr::IVRSystem* m_VRsystem;
	vr::IVRCompositor* m_VRcompositor;
	vr::IVRChaperone* m_VRChaperone;

	Callback<ApplicationEvent> m_ApplicationCallback;
};

