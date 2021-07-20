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

void File::WriteFile(const std::string& path, const Buffer& buffer)
{
#ifdef FILE_THREADED 
	std::async(std::launch::async, [&]() {
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
