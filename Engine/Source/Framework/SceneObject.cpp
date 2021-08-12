#include "SceneObject.h"
#include "DEngine.h"

SceneObject::~SceneObject()
{
	const auto& comps = GetComponents();
	for (auto& it = comps.begin(); it != comps.end();)
	{
		it = DestroyComponent(*it);
	}
}

void SceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//update components
	for (uint i = 0; i < GetComponents().size(); i++)
	{
		auto obj = GetComponents()[i];

		if (CastCheck<ObjectComponent>(obj))
			obj->OnUpdate(tick);
	}
}


void SceneObject::OnConstruct()
{
	Super::OnConstruct();

	//auto destroy object when its module is unloaded
	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{
			//unbind auto destruct from all components since its unnecessary now
			for (auto& comp : GetComponents())
			{
				comp->m_ModuleCallback.Destroy();
			}

			GetScene()->DestroySceneObject(this);
		}
	});

	GetScene()->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);

	//auto load prefab on change
	m_AssetCallback.Assign([&](AssetEvent* event)
	{
		if (IsPrefab() && event->GetEventType() == AssetEventType::ASSETSAVE)
		{
			Ref<PrefabAsset> prefabAsset = Cast<PrefabAsset>(event->GetAsset());
			if (prefabAsset && event->GetAsset()->GetID() == GetPrefabAssetRef()->GetID())
			{
				Buffer MyBuffer; //save myself then load later after the prefab loaded
				Serialize(MyBuffer);

				prefabAsset->LoadPrefab(this, false);

				Deserialize(MyBuffer);
			}
		}
	});

	GetScene()->GetApplication()->GetAssetManager().GetEventDispatcher().Bind(m_AssetCallback);
}

uint SceneObject::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, Super::Serialize(buffer))
	
	ArrayBuffer ComponentsData;
	for (auto& comp : GetComponents())
	{		
		[&comp, &ComponentsData]()
		{
			//write ID and Class name for finding the same component on deserialize
			Buffer CompPiece;
			STARTWRITE(CompPiece, 0);

			WRITE(&comp->GetID(), sizeof(UID));
			std::string compClassName = comp->GetClassType().Name;
			std::string compName = comp->GetName(); //used as the unique id for deserialization as ID's are generated on construction
			WRITESTRING(compClassName);
			WRITESTRING(compName);

			//write actual component data
			Buffer compBuffer;
			comp->Serialize(compBuffer);
			WRITEBUFFER(compBuffer);

			ComponentsData.AddPiece(CompPiece);
		}();
	}
	WRITEBUFFER(ComponentsData.MakeBuffer());

	STOPWRITE();
}

uint SceneObject::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, Super::Deserialize(buffer))

	Buffer rawComponentsData;
	READBUFFER(rawComponentsData);

	ArrayBuffer ComponentsData;
	ComponentsData.FromBuffer(rawComponentsData);

	for (auto& piece : ComponentsData.m_DataPieces)
	{
		UID compID;
		std::string compClassName;
		std::string compName;
		Buffer compData;

		[&piece, &compID, &compClassName, &compName, &compData]()
		{
			STARTREAD(piece, 0);
			READ(&compID, sizeof(UID));
			READSTRING(compClassName);
			READSTRING(compName);
			READBUFFER(compData);
		}();

		//now find a corresponding component and deserialize
		for (auto& comp : GetComponents())
		{
			if(comp->GetName() == compName && comp->GetClassType().Name == compClassName)	
				comp->Deserialize(compData);
		}
	}

	STOPREAD();
}

std::vector<Ref<ObjectComponent>>::iterator SceneObject::DestroyComponent(Ref<ObjectComponent> comp)
{
	//call pre delete event
	SceneEvent PreDeleteEvent;
	PreDeleteEvent.m_Component = comp.get();
	PreDeleteEvent.m_EventType = SceneEventType::COMPONENT_PRE_DELETE;
	PreDeleteEvent.m_ObjectClass = SceneObjectClass::OBJECTCOMPONENT;
	GetScene()->GetSceneEventDipatcher().Dispatch(PreDeleteEvent);

	//prevent component from deleting itself twice
	comp->m_ModuleCallback.Destroy();
	return m_Components.erase(std::find(m_Components.begin(), m_Components.end(), comp));
}

void SceneObject::DestroyComponent(ObjectComponent* comp)
{
	auto remove = m_Components.end();
	for (auto it = m_Components.begin(); it != m_Components.end(); it++)
	{
		if (comp == (*it).get())
		{
			remove = it;
			break;
		}
	}

	if (remove != m_Components.end())
	{
		//call pre delete event
		SceneEvent PreDeleteEvent;
		PreDeleteEvent.m_Component = (*remove).get();
		PreDeleteEvent.m_EventType = SceneEventType::COMPONENT_PRE_DELETE;
		PreDeleteEvent.m_ObjectClass = SceneObjectClass::OBJECTCOMPONENT;
		GetScene()->GetSceneEventDipatcher().Dispatch(PreDeleteEvent);

		//prevent component from deleting itself twice
		(*remove)->m_ModuleCallback.Destroy();
		m_Components.erase(remove);
	}
}

void SceneObject::AddComponent(ObjectComponent* comp)
{
	ASSERT(comp);

	//assign owner and add to object array
	comp->m_Owner = (this);
	Ref<ObjectComponent> compref = ToRef<ObjectComponent>(comp);
	m_Components.push_back(compref);

	//Must call
	comp->Initialize(ObjectInitializer(ContructFlags::RANDOMID));
}
