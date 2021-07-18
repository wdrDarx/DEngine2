#pragma once
#include "Framework\Application.h"

class EditorApp : public Application
{
public:
	EditorApp();

	void OnUpdate(const Tick& tick) override;
};

