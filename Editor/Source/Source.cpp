#include "DEngine.h"
#include "Classes/EditorApp.h"

int main()
{
	Engine engine;
	engine.AddApplication(MakeRef<EditorApp>());
	engine.Start();
	return 0;
}
