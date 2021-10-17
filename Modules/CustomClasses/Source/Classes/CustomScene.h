#pragma once
#include "CustomClasses.h"

class CUSTOMCLASSES CustomScene : public Scene
{
	public:
		OBJECT_CLASS_DEF(CustomScene, Scene);

		void OnBeginPlay() override
		{
			LogTemp("yes sir yuong");
			Super::OnBeginPlay();
		}
};