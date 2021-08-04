#include "ChildSceneObject.h"


void ChildSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	ASSERT(m_QuadComponent);
}

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
// 
//   	Image loadedimg(FileDialog::OpenFile(".png"));
//   	Ref<TextureAsset> texture1 = MakeRef<TextureAsset>(loadedimg);
//   	GetScene()->GetApplication()->GetAssetManager().SaveAsset(texture1, FileDialog::SaveFile(".TextureAsset"));
// 
// 
// 	auto texture = GetScene()->GetApplication()->GetAssetManager().LoadAsset(textureRef);
// 	GetScene()->GetApplication()->GetAssetManager().Unload(textureRef.GetAssetHandle()->GetAssetPath());

	m_QuadComponent = CreateComponent<QuadComponent>(ObjectInitializer::Module(this));
}
