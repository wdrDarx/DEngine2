#include "EditorApp.h"
#include "DEngine.h"

EditorApp::EditorApp() : Application()
{
	MakeWindow("DEditor", 1280, 720, false);
	GetWindow()->SetVsync(false);
}

void EditorApp::OnUpdate(const Tick& tick)
{
	LogWarning("FPS " + STRING((1.0 / tick.DeltaTime)));

	GetWindow()->StartFrame();
	glBegin(GL_TRIANGLES);
	glColor3f(0.1, 0.2, 0.3);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(0, 1, 0);
	glEnd();
	GetWindow()->EndFrame();
}
