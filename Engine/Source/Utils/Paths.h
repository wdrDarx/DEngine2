#pragma once
#include <Windows.h>
#include <string>

class Paths
{
public:
	static std::string GetBaseDirectory();
	static std::string GetModulesDirectory();
	static std::string GetContentDirectory();
	static std::string RemoveExtension(const std::string& path);
	static std::string MakeRelativeTo(const std::string& Base, const std::string& Path);
};

