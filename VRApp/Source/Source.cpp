#include "DEngine.h"
#include "VRApp.h"

int main()
{
	Engine engine;
	engine.AddApplication(MakeRef<VRApp>());
	engine.Start();
	return 0;
}
