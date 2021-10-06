#include "FileSystem.h"
#include "DEngine.h"
#include <windows.h>
#include <commdlg.h>
#include "ole2.h"
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

std::future<void> File::AsyncReadFile(const std::string& path, Buffer& buffer)
{
	return
	std::async(std::launch::async, [&]()
	{
		std::ifstream stream(path, std::ios::in | std::ios::binary);
		if (stream)
		{
			buffer.resize(GetFileSize(path));
			stream.read((char*)buffer.data(), buffer.size());
		}
		stream.close();
	});
}

std::string File::ReadFileAsString(const std::string& path)
{
	Buffer temp;
	ReadFile(path, temp);
	return std::string(temp.begin(), temp.end());
}

void File::WriteFileAsString(const std::string& path, const std::string& string)
{
	Buffer temp(string.begin(), string.end());
	WriteFile(path, temp);
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

std::string File::WideStringToString(const std::wstring& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(str);
}

void File::WriteFile(const std::string& path, const Buffer& buffer)
{
	std::ofstream stream(path, std::ios::out | std::ios::binary);
	if (stream)
	{
		stream.write((char*)buffer.data(), buffer.size());
	}
	stream.close();
}

std::future<void> File::AsyncWriteFile(const std::string& path, const Buffer& buffer)
{
	return 
	std::async(std::launch::async, [&]()
	{

		std::ofstream stream(path, std::ios::out | std::ios::binary);
		if (stream)
		{
			stream.write((char*)buffer.data(), buffer.size());
		}
		stream.close();
	});
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
	if (FILE* file = fopen(path.c_str(), "r")) {
		fclose(file);
		return true;
	}
	else {
		return false;
	}
}

bool File::DoesPathExist(const std::string& path)
{
	return std::filesystem::exists(path);
}

void File::MakePath(const std::string& path)
{
	if (!DoesPathExist(path))
	{
		std::filesystem::create_directory(std::filesystem::path(path));
	}
}

void File::AcceptDragFiles(Window* targetWindow, DropTarget* dropTarget)
{
	OleInitialize(NULL);
	DragAcceptFiles(glfwGetWin32Window(targetWindow->GetGlfwWindow()), true);
	auto result = RegisterDragDrop(glfwGetWin32Window(targetWindow->GetGlfwWindow()), dropTarget);

	if(result < 0)
		LogError("Failed to register Drop target! " + STRING(result));
}


HRESULT DropTarget::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{

	return HRESULT();
}

HRESULT DropTarget::DragLeave()
{
	return HRESULT();
}

HRESULT DropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return HRESULT();
}

HRESULT DropTarget::Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	FORMATETC fdrop = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	if (SUCCEEDED(pDataObj->QueryGetData(&fdrop)))
	{
		STGMEDIUM stgMedium = { 0 };
		stgMedium.tymed = TYMED_HGLOBAL;
		HRESULT hr = pDataObj->GetData(&fdrop, &stgMedium);
		if (SUCCEEDED(hr))
		{
			HGLOBAL gmem = stgMedium.hGlobal;
			HDROP hdrop = (HDROP)GlobalLock(gmem);
			UINT numOfFiles = DragQueryFile((HDROP)hdrop, 0xFFFFFFFF, NULL, 0);
			TCHAR buffer[MAX_PATH];
			for (int i = 0; i < numOfFiles; i++)
			{
				UINT charsCopied = DragQueryFile((HDROP)hdrop, i, buffer, MAX_PATH);
// 				wchar_t* name = L"File : ";
// 				MessageBox(NULL, buffer, name, MB_OK);

				//call if a callback is assigned (the string is the full path to the file dropped in)
				if(m_Callback)
				{ 
					m_Callback(File::WideStringToString(buffer));
				}
			}
			GlobalUnlock(gmem);
		}
	}
	return HRESULT();
}

HRESULT DropTarget::QueryInterface(REFIID iid, void** ppvObject)
{
	return HRESULT();
}

ULONG DropTarget::AddRef(void)
{
	return ULONG();
}

ULONG DropTarget::Release(void)
{
	return ULONG();
}
