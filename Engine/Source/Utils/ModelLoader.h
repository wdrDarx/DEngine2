#pragma once
#include "Core/Core.h"

class Mesh;
class ModelLoader
{
public:
	static void LoadOBJ(const std::string& fullpath, Mesh* mesh);
	//static void LoadFBX(const std::string& fullpath, StaticMesh* mesh);
};