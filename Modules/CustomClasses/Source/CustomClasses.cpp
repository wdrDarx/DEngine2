#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"

void CustomClasses::OnLoad()
{
	LogWarning("Loaded CustomClasses Module");
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, RegistryType::OBJECT);
}

void CustomClasses::OnUnload()
{
	LogWarning("Unloaded CustomClasses Module");
}
