#pragma once
#include "Core/Core.h"

struct DENGINE_API DataObject
{
	DataObject(std::type_index t, const Ref<void>& d) : type(t), ptr(d) {};
	const std::type_index type;
	Ref<void> ptr;
};

class DENGINE_API  DataCache
{
public:
	template<typename T>
	void Set(const std::string& tag, const Ref<void>& data)
	{	
		bool create = true;
		std::pair<const std::string, DataObject>* found = nullptr;
		for (auto& element : m_ObjectCache)
		{
			if (tag == element.first)
			{
				std::type_index type = typeid(T);
				if (type == element.second.type)
				{ 
					found = &element;
					create = false;
				}
			}
		}

		if(create)
			m_ObjectCache.insert(std::make_pair(tag, DataObject(typeid(T), data)));
		else
			found->second.ptr = data;		
	}

	template<typename T>
	void Remove(const std::string& tag)
	{
		m_ObjectCache.erase(tag);
	}
	
	template<typename T, typename... Args>
	T* Add(const std::string& tag, Args&&... args)
	{
		T* object = new T(std::forward<Args>(args)...);
		m_ObjectCache.insert(std::make_pair(tag, DataObject(typeid(T), (void*)object)));
		return object;
	}

	void Clear()
	{
		m_ObjectCache.clear();
	}
	
	template<typename T>
	const Ref<T> Get(const std::string& tag) const
	{
		for (const auto& element : m_ObjectCache)
		{
			if (tag == element.first)
			{
				std::type_index type = typeid(T);
				if(type == element.second.type)
					return  std::static_pointer_cast<T>(element.second.ptr);
			}
		}
		//LogWarning("NO OBJECT FOUND IN CACHE!");
		return nullptr;
	}

private:
	 std::unordered_map<std::string, DataObject> m_ObjectCache;
};

