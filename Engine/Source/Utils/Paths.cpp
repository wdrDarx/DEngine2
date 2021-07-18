#pragma once
#include "Paths.h"
#include <filesystem>
#include <iostream>

std::string remove_extension(const std::string& filename) 
{
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

std::string Paths::GetBaseDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");

	return std::string(buffer).substr(0, pos);
}

std::string Paths::GetContentDirectory()
{
	return GetBaseDirectory() + "\\Content\\";
}

std::string Paths::RemoveExtension(const std::string& path)
{
	return remove_extension(path);
}

std::string Paths::MakeRelativeTo(const std::string& Base, const std::string& Path)
{
	std::filesystem::path base(Base);
	std::filesystem::path path(Path);

	return  std::filesystem::relative(path, base).generic_string();
}
