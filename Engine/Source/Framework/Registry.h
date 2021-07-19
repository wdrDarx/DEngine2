#pragma once
#include "Core/Core.h"
#include "ObjectBase.h"

#define REGISTER(RegistryRef, ObjectClass, ModuleClass, RegistryType) RegistryRef.Register<ObjectClass>({RegistryType, #ObjectClass, #ModuleClass});
#define UNREGISTER(RegistryRef, ObjectClass, ModuleClass, RegistryType) RegistryRef.Unregister<ObjectClass>({RegistryType, #ObjectClass, #ModuleClass});

enum class DENGINE_API RegistryType
{
	OBJECT = 0,
	APPOBJECT,
	COMPONENT 
};

struct DENGINE_API RegisterKey
{
	RegistryType type;
	std::string name;
	std::string AssignedModuleName;

	bool operator==(const RegisterKey& other) const
	{
		return (type == other.type && name == other.name && AssignedModuleName == other.AssignedModuleName);
	}
};

namespace std 
{
	template <>
	struct hash<RegisterKey>
	{
		std::size_t operator()(const RegisterKey& k) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			return ((hash<int>()((int)k.type)
				^ (hash<string>()(k.name) << 1)) >> 1)			
				^ (hash<string>()(k.AssignedModuleName) << 1);				
		}
	};

}

template<typename Key, typename T, typename... ConstructionArgs>
class DENGINE_API _RegistryBase
{
public:
	using Instantiator = T* (*)(ConstructionArgs...);
protected:
	template<typename U>
	static T* createInstance(ConstructionArgs... args)
	{
		return new U(std::forward<ConstructionArgs>(args)...);
	}
	using Instantiators = std::unordered_map<Key, Instantiator>;
	Instantiators subclassInstantiators;

public:

	std::vector<Key> GetRegisteredKeys() const 
	{
		std::vector<Key> temp;
		for(auto it = subclassInstantiators.begin(); it != subclassInstantiators.end(); it++)
		{
			temp.push_back(it->first);
		}
		return temp;
	}

	template<typename U>
	void Register(const Key& key)
	{
		static_assert(std::is_base_of<T, U>::value, "Cant register this class");
		auto it = subclassInstantiators.find(key);
		if (it != subclassInstantiators.end())
		{
			//registered twice
		}
		subclassInstantiators.emplace(key, &createInstance<U>);
	}

	template<typename U>
	void Unregister(const Key& key)
	{
		static_assert(std::is_base_of<T, U>::value, "Cant unregister this class");
		auto it = subclassInstantiators.find(key);
		if (it != subclassInstantiators.end())
		{
			subclassInstantiators.erase(it);
		}
		else
		{
			LogWarning("Class isnt registered");
			//
		}

	}

public:
	T* Make(const Key& subclass_key, ConstructionArgs... args) const
	{
		auto it = subclassInstantiators.find(subclass_key);
		if (it == subclassInstantiators.end())
		{
			return nullptr;
		}
		auto instantiator = it->second;
		T* obj = instantiator(std::forward<ConstructionArgs>(args)...);

		//set the assigned module name for the object
		obj->SetAssignedModuleName(subclass_key.AssignedModuleName);
		return obj;
	}

	bool canProduce(const Key& subclass_key) const
	{
		return subclassInstantiators.find(subclass_key) != subclassInstantiators.end();
	}
};

using Registry = _RegistryBase<RegisterKey, ObjectBase>;