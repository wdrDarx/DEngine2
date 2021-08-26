#pragma once
#include "Rendering/Pipeline.h"

class DENGINE_API DefaultPipeline : public Pipeline
{
	public:
		OBJECT_CLASS_DEF(DefaultPipeline, Pipeline)

		void OnConstruct() override;

};