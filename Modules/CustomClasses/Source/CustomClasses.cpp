#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"

void CustomClasses::OnLoad()
{
	//LogTemp("Loaded CustomClasses Module");
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
}

void CustomClasses::OnUnload()
{
	//LogTemp("Unloaded CustomClasses Module");
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
}
