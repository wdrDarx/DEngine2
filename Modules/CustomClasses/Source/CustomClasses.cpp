#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"

void CustomClasses::OnLoad()
{
	LogTemp("Loaded CustomClasses Module");
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, RegistryType::OBJECT);

	Ref<CustomAppObject> obj1 =  MakeRef<CustomAppObject>();
	obj1->Initialize(ObjectInitializer("obj1", ContructFlags::RANDOMID));

	Ref<CustomAppObject> obj2 =  MakeRef<CustomAppObject>();
	obj2->Initialize(ObjectInitializer("obj1", ContructFlags::RANDOMID));
	obj1->testProp = 69;

	if (obj1->GetProperties()[0].m_Flags & PropFlags::EditAnywhere)
	{
		int i = 0;
	}

	if (obj1->GetProperties()[0].m_Flags & PropFlags::NoSerialize)
	{
		int i = 0;
	}

	Buffer test;
	obj1->Serialize(test);
	obj2->Deserialize(test);


}

void CustomClasses::OnUnload()
{
	LogTemp("Unloaded CustomClasses Module");
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, RegistryType::OBJECT);
}
