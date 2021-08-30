#pragma once
#include "DEngine.h"
//openvr
#include "openvr.h"

class VRApp : public Application
{
public:
	VRApp();
	using Super = Application;

	void OnUpdate(const Tick& tick) override;

	Ref<Scene> m_openScene;

	Ref<Camera> m_Camera;

	Ref<Camera> m_LeftEyeCam;
	Ref<Camera> m_RightEyeCam;

	Ref<FrameBuffer> m_LeftEyeBuffer;
	Ref<FrameBuffer> m_RightEyeBuffer;

	static glm::mat4 Mat4From3x4VR(const vr::HmdMatrix34_t& mat);
	static glm::mat4 Mat4From4VR(const vr::HmdMatrix44_t& mat);

	Transform m_HeadsetTransform;
	const float m_VRScale = 100.f;
	glm::mat4 m_VRScaleMat;

	Ref<SceneObject> m_LeftController;
	Ref<SceneObject> m_RightController;

	glm::mat4 m_LeftEyeProj;
	glm::mat4 m_RightEyeProj;

	vr::IVRSystem* m_VRsystem;
	vr::IVRCompositor* m_VRcompositor;
	vr::IVRChaperone* m_VRChaperone;
	bool DebugMode = false;
};
