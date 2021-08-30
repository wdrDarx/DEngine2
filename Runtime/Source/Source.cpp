#include "DEngine.h"
#include "RuntimeApp.h"

int main()
{
	Engine engine;
	engine.AddApplication(MakeRef<RuntimeApp>());
	engine.Start();
	return 0;
}
