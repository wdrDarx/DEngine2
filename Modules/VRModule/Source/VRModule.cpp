#include "VRModule.h"
#include "Classes/VRPawn.h"

void VRModule::OnLoad()
{
	//assign App
	Module::AssignApplication(GetApplication());

	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
	}

	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), VRPawn, VRModule);

	GetApplication()->GetAssetManager().MountContentDirectory(Paths::GetModuleContentDirectory("VRModule"));
}

void VRModule::OnUnload()
{
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), VRPawn, VRModule);
	GetApplication()->GetAssetManager().UnmountContentDirectory(Paths::GetModuleContentDirectory("VRModule"));
}

void VRModule::OnUpdate(float DeltaTime)
{
	
}


glm::mat4 VRModule::Mat4From4VR(const vr::HmdMatrix44_t& mat)
{
	glm::mat4 out;
	out[0][0] = mat.m[0][0]; out[1][0] = mat.m[0][1]; out[2][0] = mat.m[0][2]; out[3][0] = mat.m[0][3];
	out[0][1] = mat.m[1][0]; out[1][1] = mat.m[1][1]; out[2][1] = mat.m[1][2]; out[3][1] = mat.m[1][3];
	out[0][2] = mat.m[2][0]; out[1][2] = mat.m[2][1]; out[2][2] = mat.m[2][2]; out[3][2] = mat.m[2][3];
	out[0][3] = mat.m[3][0]; out[1][3] = mat.m[3][1]; out[2][3] = mat.m[3][2]; out[3][3] = mat.m[3][3];

	return out;
}

glm::mat4 VRModule::Mat4From3x4VR(const vr::HmdMatrix34_t& mat)
{
	glm::mat4 out;
	out[0][0] = mat.m[0][0]; out[1][0] = mat.m[0][1]; out[2][0] = mat.m[0][2]; out[3][0] = mat.m[0][3];
	out[0][1] = mat.m[1][0]; out[1][1] = mat.m[1][1]; out[2][1] = mat.m[1][2]; out[3][1] = mat.m[1][3];
	out[0][2] = mat.m[2][0]; out[1][2] = mat.m[2][1]; out[2][2] = mat.m[2][2]; out[3][2] = mat.m[2][3];
	out[0][3] = 0;			 out[1][3] = 0;           out[2][3] = 0;           out[3][3] = 1;

	return out;
}
