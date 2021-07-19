#pragma once
#include "ObjectBase.h"

class Application;

/*An Object existant in the scope of an app with a reference to it
* Must call Initialize(const ObjectInitializer& initializer) mannually after constructing
* 
* 
*/
class DENGINE_API AppObject : public ObjectBase
{
public:
	OBJECT_CLASS_DEF(AppObject, ObjectBase)

	//override constructor receiving a reference to the app
	AppObject(Ref<Application> app) : ObjectBase(), m_Application(app)
	{

	}

	Ref<Application> GetApplication() const
	{
		return m_Application;
	}

private:
	Ref<Application> m_Application;
};