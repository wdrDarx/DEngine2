#pragma once
#include "DEngine.h"

class EditorApp;
class ContentBrowser
{
public:

	void Render(EditorApp* m_App);
	void Init();

	std::filesystem::path m_CurrentPath = Paths::GetContentDirectory();
	float m_Padding = 4.f;
	float m_ThumbnailSize = 64.f;
	Ref<Texture> m_FolderIcon;
	Ref<Texture> m_FileIcon;
	bool m_FlagRediscover = false;
	bool m_IsDragging = false;
	std::string m_RenamingPath;
	std::string m_FileOptionsPath;
};