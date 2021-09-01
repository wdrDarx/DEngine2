#include "DefaultPipeline.h"
#include "DEngine.h"

void DefaultPipeline::OnConstruct()
{
	Super::OnConstruct();

	CreateRenderer<CubemapRenderer>(ObjectInitializer());
	CreateRenderer<QuadRenderer>(ObjectInitializer());
	CreateRenderer<MeshRenderer>(ObjectInitializer());
	CreateRenderer<DebugRenderer>(ObjectInitializer());
}

