#pragma once
#include "DEngine.h"

class EditorApp;
class MenuBar
{
public:
	void Init();
	void Render(EditorApp* m_App);
	Ref<Texture> m_PlayIcon;
	Ref<Texture> m_StopIcon;

	float m_IconSize = 20.f;
};