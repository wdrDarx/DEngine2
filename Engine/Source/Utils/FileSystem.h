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
		static void WriteFile(const std::string& path, const Buffer& buffer);
		static int GetFileSize(const std::string& path);
};

