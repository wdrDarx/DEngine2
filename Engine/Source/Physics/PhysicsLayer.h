#pragma once
#include "Core/Core.h"
#include "Framework/EnumBase.h"

struct PhysicsLayer
{
	uint LayerID;

	std::string Name;
	uint BitValue;
	int Blocking = 0;
	int Overlapping = 0;

	bool IsValid() const
	{
		return !Name.empty() && BitValue > 0;
	}
};

class DENGINE_API PhysicsLayerManager
{
public:
	uint AddLayer(const std::string& name, bool BlockAll = false, bool OverlappAll = false);
	void RemoveLayer(uint layerId);

	void SetLayerCollision(uint layerId, uint otherLayer, bool shouldBlock, bool shouldOverlap);

	std::vector<PhysicsLayer> GetBlockingLayers(uint layerId);
	std::vector<PhysicsLayer> GetOverlappingLayers(uint layerId);

	const std::vector<PhysicsLayer>& GetLayers() 
	{
		return m_Layers; 
	}

	static const std::vector<std::string>& GetLayerNames();

	PhysicsLayer& GetLayer(uint32_t layerId);
	PhysicsLayer& GetLayer(const std::string& layerName);

	uint GetLayerCount() 
	{ 
		return uint(m_Layers.size());
	}

	bool ShouldBlock(uint layer1, uint layer2);
	bool ShouldOverlap(uint layer1, uint layer2);

	bool IsLayerValid(uint layerId);

private:
	uint GetNextLayerID();

private:
	std::vector<PhysicsLayer> m_Layers;
	static std::vector<std::string> s_LayerNames;
	PhysicsLayer s_NullLayer = { 0, "NULL", 0, -1 };
};


struct DENGINE_API LayerCollisions : public EnumBase
{
	enum Enum
	{
		Layer_0 = BIT(1),
		Layer_1 = BIT(2),
		Layer_2 = BIT(3),
		Layer_3 = BIT(4),
		Layer_4 = BIT(5),
		Layer_5 = BIT(6)
	};

	BITMASK_DEF_BEGIN(LayerCollisions, Enum)
		ENUM_DEF(Layer_0);
		ENUM_DEF(Layer_1);
		ENUM_DEF(Layer_2);
		ENUM_DEF(Layer_3);
		ENUM_DEF(Layer_4);
		ENUM_DEF(Layer_5);

		auto Names = PhysicsLayerManager::GetLayerNames();
		if (Names.size() > 0)
		{
			uint InitialSize = _StringMap.size();
			_StringMap.clear();
			for (uint i = 0; i < InitialSize; i++)
			{
				if (i >= Names.size())
					return _StringMap;

				_StringMap[Names[i]] = BIT(i + 1);
			}
		}
	ENUM_DEF_END()
};

struct DENGINE_API CollisionLayerEnum : public EnumBase
{
	enum Enum
	{
		Layer_0 = 0,
		Layer_1,
		Layer_2,
		Layer_3,
		Layer_4,
		Layer_5
	};

	ENUM_DEF_BEGIN(CollisionLayerEnum, Enum)
		ENUM_DEF(Layer_0);
		ENUM_DEF(Layer_1);
		ENUM_DEF(Layer_2);
		ENUM_DEF(Layer_3);
		ENUM_DEF(Layer_4);
		ENUM_DEF(Layer_5);

		auto Names = PhysicsLayerManager::GetLayerNames();
		if (Names.size() > 0)
		{
			uint InitialSize = _StringMap.size();
			_StringMap.clear();
			for (uint i = 0; i < InitialSize; i++)
			{
				if(i >= Names.size())
					return _StringMap;

				_StringMap[Names[i]] = i;
			}
		}
	ENUM_DEF_END()
};


