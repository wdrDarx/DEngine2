#pragma once
#include "Core/Core.h"

struct DataObject
{
	DataObject (Ref<void> d) : ptr(d) 
	{
		
	}

	DataObject()
	{
	
	}

	~DataObject()
	{
		
	}

	Ref<void> ptr = nullptr;
};

/*
	class for storing Data pieces retrievable with a tag
*/
class DataCache
{
public:

	void Set(const std::string& tag, Ref<void> data)
	{	
		//create new data object
		//m_ObjectCache.insert(std::make_pair(tag, DataObject(data)));
		m_ObjectCache[tag] = DataObject(data);
	}

	void Remove(const std::string& tag)
	{
		m_ObjectCache.erase(tag);
	}

	void Clear()
	{
		m_ObjectCache.clear();
	}
	
	Ref<void> Get(const std::string& tag) const
	{
		for (const auto& element : m_ObjectCache)
		{
			if (tag == element.first)
			{
				return element.second.ptr;
			}
		}

		return nullptr;
	}

private:
	 std::unordered_map<std::string, DataObject> m_ObjectCache;
};

