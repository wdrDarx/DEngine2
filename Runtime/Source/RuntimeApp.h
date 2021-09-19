#pragma once
#include "DEngine.h"

class RuntimeApp : public Application
{
public:
	RuntimeApp();
	using Super = Application;

	void OnUpdate(const Tick& tick) override;

	Ref<Scene> m_openScene;
	Ref<Camera> m_Camera;
	Ref<FrameBuffer> m_ScreenFrameBuffer;

	bool loaded = false;
};
