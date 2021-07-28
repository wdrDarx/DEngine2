#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"

#define FILE_THREADED 

class DENGINE_API FileDialog
{
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};

class DENGINE_API File
{
	public:
		static void ReadFile(const std::string& path, Buffer& buffer);
		static void ReadFile(const std::string& path, Buffer& buffer, const size_t& bytesToRead);
		static std::string GetFileExtenstionFromPath(const std::string& path);
		static std::string RemoveFileExtensionFromPath(const std::string& path);
		static std::string GetFileNameFromPath(const std::string& path);
		static void WriteFile(const std::string& path, const Buffer& buffer);
		static int GetFileSize(const std::string& path);
		static bool DoesFileExist(const std::string& path);
};

