#include "QuadComponent.h"
#include "QuadRenderer.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	Buffer thisBuffer;
	Serialize(thisBuffer);
	File::WriteFile(Paths::GetBaseDirectory() + "\\test.txt", thisBuffer);
	Deserialize(thisBuffer);

	if(auto asset = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(QuadTexture))
		GetOwner()->GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ QuadSize.x, QuadSize.y,1 }, QuadTransform, QuadColor, asset->GetTexture());
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();

// 	TestStruct test;
// 	test.porp = 21;
// 
// 	TestStruct test2;
// 	test2.porp = 11;
	std::string test = "batman";
	m_TestArray.Add(test);
	m_TestArray.Add(test);
// 	m_TestArray.Add(test2);
// 	m_TestArray.Add(test);
// 
// 	for (uint i = 0; i < m_TestArray.m_InternalArray.size(); i++)
// 	{
// 		TestStruct test = m_TestArray[i];
// 		LogTemp(Log::string(test.porp));
// 	}
// 
	
// 
// 	for (uint i = 0; i < m_TestArray.m_InternalArray.size(); i++)
// 	{
// 		TestStruct test = m_TestArray[i];
// 		LogTemp(Log::string(test.porp));
// 	}
}
