#include "CustomSceneObject.h"
#include "CustomAppObject.h"

void CustomSceneObject::OnConstruct()
{
	Super::OnConstruct();

// 	MethodFuncPtr<float> test;
// 	test.FuncPtr = std::bind(&CustomSceneObject::testfunc, this, std::placeholders::_1);
// 
// 	std::vector<Property*> args{ &m_Properties[2] };
// 	test.TryCall(args);
}

void CustomSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
//  	Buffer buffer;
//  	Serialize(buffer);
//  	File::WriteFile(Paths::GetBaseDirectory() + "//Test.txt", buffer);
//  	Deserialize(buffer);	
}

void CustomSceneObject::testfunc(float test)
{
	LogTemp(STRING(test));
}
