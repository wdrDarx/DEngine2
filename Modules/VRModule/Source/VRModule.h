#pragma once
#include "Framework/Module.h"
#include "CustomClasses.h"
#include "openvr.h"
#include "DEngine.h"

#define VRMODULE_API

#ifdef VRMODULE_API
#define VRMODULE __declspec(dllexport)
#else
#define VRMODULE
#endif

class VRMODULE VRModule : public Module
{
public:
	using Module::Module;

	//helper functions
	static glm::mat4 Mat4From3x4VR(const vr::HmdMatrix34_t& mat);
	static glm::mat4 Mat4From4VR(const vr::HmdMatrix44_t& mat);

	void OnLoad() override;
	void OnUnload() override;
	void OnUpdate(float DeltaTime) override;
};

