#include "FileSystem.h"

#include <windows.h>
#include <commdlg.h>
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

std::string FileDialog::OpenFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = {0};

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";

	std::string fileNameStr;

	if (GetOpenFileNameA(&ofn) == TRUE)
		fileNameStr = szFile;

	return fileNameStr;
}

std::string FileDialog::SaveFile(const char* filter)
{
	OPENFILENAMEA ofn;
	CHAR szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = "";

	std::string fileNameStr;

	if (GetSaveFileNameA(&ofn) == TRUE)
		fileNameStr = szFile;

	return fileNameStr;
}

std::string remove_extension(const std::string& filename)
{
	size_t lastdot = filename.find_last_of(".");
	if (lastdot == std::string::npos) return filename;
	return filename.substr(0, lastdot);
}

void File::ReadFile(const std::string& path, Buffer& buffer)
{
	std::ifstream stream(path, std::ios::in | std::ios::binary);
	if (stream)
	{
		buffer.resize(GetFileSize(path));
		stream.read((char*)buffer.data(), buffer.size());
	}
	stream.close();

}

void File::ReadFile(const std::string& path, Buffer& buffer, const size_t& bytesToRead /*= 0*/)
{
	std::ifstream stream(path, std::ios::in | std::ios::binary);
	if (stream)
	{
		buffer.resize(bytesToRead);
		stream.read((char*)buffer.data(), bytesToRead);
	}
	stream.close();
}

std::string File::GetFileExtenstionFromPath(const std::string& path)
{
	size_t lastdot = path.find_last_of(".");
	if (lastdot == std::string::npos) return "";

	return Substring(path, lastdot + 1, path.length());
}

std::string File::RemoveFileExtensionFromPath(const std::string& path)
{
	return remove_extension(path);
}

std::string File::GetFileNameFromPath(const std::string& path)
{
	return Substring(path, path.find_last_of("\\") + 1, path.find_last_of("."));
}

std::wstring File::StringToWideString(const std::string& str)
{
	int len;
	int slength = (int)str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void File::WriteFile(const std::string& path, const Buffer& buffer)
{
#ifdef FILE_THREADED 
	std::future<void> ftr = std::async(std::launch::async, [&]() {
#endif

	std::ofstream stream(path, std::ios::out | std::ios::binary);
	if (stream)
	{
		stream.write((char*)buffer.data(), buffer.size());
	}
	stream.close();

#ifdef FILE_THREADED 
		});
#endif
}

int File::GetFileSize(const std::string& path)
{
	FILE* p_file = NULL;
	p_file = fopen(path.c_str(), "rb");
	fseek(p_file, 0, SEEK_END);
	int size = ftell(p_file);
	fclose(p_file);
	return size;
}

bool File::DoesFileExist(const std::string& path)
{
	std::ifstream f(path.c_str());
	return f.good();
}
