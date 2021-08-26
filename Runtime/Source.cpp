#include "DEngine.h"
#include "Source/RuntimeApp.h"

int main()
{
	Engine engine;
	engine.AddApplication(MakeRef<RuntimeApp>());
	engine.Start();
	return 0;
}
