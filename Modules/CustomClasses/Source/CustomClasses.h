#pragma once
#include "Framework/Module.h"

#define CUSTOMCLASSES_API

#ifdef CUSTOMCLASSES_API
#define CUSTOMCLASSES __declspec(dllexport)
#else
#define CUSTOMCLASSES
#endif

class CUSTOMCLASSES CustomClasses : public Module
{
public:
	using Module::Module;

	void OnLoad() override;
	void OnUnload() override;
};

