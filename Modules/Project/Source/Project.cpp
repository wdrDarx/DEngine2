#include "Project.h"

void Project::OnLoad()
{
	//assign App
	Module::AssignApplication(GetApplication());

	if (!gladLoadGL())
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
	}

	File::MakePath(Paths::GetModuleContentDirectory("Project"));
	GetApplication()->GetAssetManager().MountContentDirectory(Paths::GetModuleContentDirectory("Project"));
}

void Project::OnUnload()
{

	
}

void Project::OnUpdate(float DeltaTime)
{
	
}

EXTERN PROJECT MODULEDEF(Project)

