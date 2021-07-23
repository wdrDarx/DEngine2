#pragma once
#include "Core/Core.h"
#include "ObjectBase.h"
#include "Core/Allocator.h"

#define REGISTER(RegistryRef, ObjectClass, ModuleClass, ObjectClassType) RegistryRef.Register<ObjectClass>({ObjectClassType, #ObjectClass, #ModuleClass});
#define UNREGISTER(RegistryRef, ObjectClass, ModuleClass, ObjectClassType) RegistryRef.Unregister<ObjectClass>({ObjectClassType, #ObjectClass, #ModuleClass});


struct RegisterKey
{
	ObjectClassType type;
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

class Application;

template<typename Key, typename T, typename... ConstructionArgs>
class _RegistryBase
{
public:
	using Instantiator = T* (*)(ConstructionArgs...);

protected:
	template<typename U>
	static T* createInstance(ConstructionArgs... args)
	{
		
#if 0
		//trying to allocate on the Engine module
		Allocator alloc;
		U* ptr = (U*)alloc.Allocate(sizeof(U));
		std::allocator<U> stdalloc;
		stdalloc.construct(ptr, std::forward<ConstructionArgs>(args)...);
		return ptr;
#else
		return new U(std::forward<ConstructionArgs>(args)...);
#endif
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
		obj->SetAssociatedModuleName(subclass_key.AssignedModuleName);

		return obj;
	}

	T* MakeAmbiguous(const std::string& FriendlyClassName, ConstructionArgs... args) const
	{
		//cant do this if the key isnt a RegisterKey
		if constexpr (!std::is_same<Key, RegisterKey>::value) ASSERT(false);

		for (auto it = subclassInstantiators.begin(); it != subclassInstantiators.end(); it++)
		{
			if ((*it).first.name == FriendlyClassName)
			{
				auto instantiator = it->second;
				T* obj = instantiator(std::forward<ConstructionArgs>(args)...);

				//set the assigned module name for the object
				obj->SetAssociatedModuleName((*it).first.AssignedModuleName);

				return obj;
			}
		}

		//couldnt find a registered class
		return nullptr;
	}

	bool canProduce(const Key& subclass_key) const
	{
		return subclassInstantiators.find(subclass_key) != subclassInstantiators.end();
	}

	Ref<Application> m_App;
};

using Registry = _RegistryBase<RegisterKey, ObjectBase>;