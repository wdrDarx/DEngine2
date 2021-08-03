#include "QuadComponent.h"
#include "QuadRenderer.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

// 	Buffer thisBuffer;
// 	Serialize(thisBuffer);
// 	File::WriteFile(Paths::GetBaseDirectory() + "\\test.txt", thisBuffer);
// 	Deserialize(thisBuffer);

	if (auto asset = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(QuadTexture))
	{
		for(uint i = 0; i < m_Quads.Size(); i++)
			GetOwner()->GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ QuadSize.x, QuadSize.y,1 }, m_Quads[i], QuadColor, asset->GetTexture());
	}
	
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();

}
