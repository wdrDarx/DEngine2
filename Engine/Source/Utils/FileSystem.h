#pragma once
#include "Core/Core.h"
#include "Serialization/Buffer.h"

#define FILE_THREADED 

/*
	Class for opening file dialogs
*/
class DENGINE_API FileDialog
{
public:
	static std::string OpenFile(const char* filter);
	static std::string SaveFile(const char* filter);
};


class DropTarget : public IDropTarget
{
public:
	HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
	HRESULT DragLeave() override;
	HRESULT DragOver(DWORD  grfKeyState, POINTL pt, DWORD* pdwEffect) override;
	HRESULT Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect) override;
	
	// IUnknown members
	HRESULT QueryInterface(REFIID iid, void** ppvObject);
	ULONG  AddRef(void);
	ULONG  Release(void);


	std::function<void(const std::string&)> m_Callback;
};


/*
	Utility class for the filesystem
*/
class Window;
class DENGINE_API File
{
	public:
		static void ReadFile(const std::string& path, Buffer& buffer);
		static void ReadFile(const std::string& path, Buffer& buffer, const size_t& bytesToRead);
		static std::string GetFileExtenstionFromPath(const std::string& path);
		static std::string RemoveFileExtensionFromPath(const std::string& path);
		static std::string GetFileNameFromPath(const std::string& path);
		static std::wstring StringToWideString(const std::string& str);
		static std::string WideStringToString(const std::wstring& str);
		static void WriteFile(const std::string& path, const Buffer& buffer);
		static int GetFileSize(const std::string& path);
		static bool DoesFileExist(const std::string& path);
		static bool DoesPathExist(const std::string& path);
		static void MakePath(const std::string& path);

		/*
			Start accepting dragged in files to a window
			set the m_Callback to a custom function, the string input is the full path to a dragged-in file
		*/
		static void AcceptDragFiles(Window* targetWindow, DropTarget* dropTarget);
};

