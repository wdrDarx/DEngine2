#include "CustomSceneObject.h"
#include "CustomAppObject.h"

void CustomSceneObject::OnConstruct()
{
	Super::OnConstruct();
	LogTemp("CustomSceneObject constructed!");

	if(Rand::Float() < 0.99)
		GetScene()->CreateSceneObject<CustomSceneObject>(ObjectInitializer::Module(this));
}

void CustomSceneObject::OnUpdate(const Tick& tick)
{
// 	Buffer buffer;
// 	Serialize(buffer);
// 	File::WriteFile(Paths::GetBaseDirectory() + "//Test.txt", buffer);
}
