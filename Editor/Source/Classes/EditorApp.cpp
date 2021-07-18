#include "EditorApp.h"
#include "Event/Event.h"

EditorApp::EditorApp() : Application()
{
	MakeWindow("DEditor", 1280, 720, false);
	GetWindow()->SetVsync(false);

	m_Callback.Assign([&](niggaEvent* event)
	{
		LogWarning(event->message);
	});

	m_Callback2.Assign([&](niggaEvent2* event)
	{
		LogWarning(event->message + " 2");
	});

	m_EventDispatcher.Bind(m_Callback);
	m_EventDispatcher.Bind(m_Callback2);

	m_EventDispatcher.Dispatch(niggaEvent("uhh yeah"));
	m_EventDispatcher.Dispatch(niggaEvent2("yayay"));

	CreateAppObject<AppObject>();
}

void EditorApp::OnUpdate(const Tick& tick)
{
	//LogWarning("FPS " + STRING((1.0 / tick.DeltaTime)));
	GetWindow()->SetCurrentContext();
	GetWindow()->StartFrame();

	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 0.2, 0.3);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	glEnd();

	GetWindow()->EndFrame();
}
