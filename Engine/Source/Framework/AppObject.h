#pragma once
#include "ObjectBase.h"

class Application;

//An Object existant in the scope of an app with a reference to it
class DENGINE_API AppObject : public ObjectBase
{
public:
	//override constructor receiving a reference to the app
	AppObject(const ObjectInitializer& initializer, Ref<Application> app) : ObjectBase(initializer), m_Application(app)
	{

	}

	Ref<Application> GetApplication() const
	{
		return m_Application;
	}

private:
	Ref<Application> m_Application;
};