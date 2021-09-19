#include "PhysicsLayer.h"
#include "DEngine.h"

template<typename T, typename ConditionFunction>
static bool RemoveIfExists(std::vector<T>& vector, ConditionFunction condition)
{
	for (std::vector<T>::iterator it = vector.begin(); it != vector.end(); ++it)
	{
		if (condition(*it))
		{
			vector.erase(it);
			return true;
		}
	}

	return false;
}

uint PhysicsLayerManager::AddLayer(const std::string& name, bool BlockAll /*= false*/, bool OverlappAll /*= false*/)
{
	uint layerId = GetNextLayerID();
	PhysicsLayer layer = { layerId, name, BIT(layerId), BIT(layerId) };
	m_Layers.insert(m_Layers.begin() + layerId, layer);

	for (const auto& layer2 : m_Layers)
	{
		SetLayerCollision(layer.LayerID, layer2.LayerID, BlockAll, OverlappAll);
	}

	return layer.LayerID;
}

void PhysicsLayerManager::RemoveLayer(uint layerId)
{
	PhysicsLayer& layerInfo = GetLayer(layerId);

	for (auto& otherLayer : m_Layers)
	{
		if (otherLayer.LayerID == layerId)
			continue;

		if (otherLayer.Blocking & layerInfo.BitValue)
		{
			otherLayer.Blocking &= ~layerInfo.BitValue;
		}

		if (otherLayer.Overlapping & layerInfo.BitValue)
		{
			otherLayer.Overlapping &= ~layerInfo.BitValue;
		}
	}

	RemoveIfExists<PhysicsLayer>(m_Layers, [&](const PhysicsLayer& layer) { return layer.LayerID == layerId; });
}

void PhysicsLayerManager::SetLayerCollision(uint layerId, uint otherLayer, bool shouldBlock, bool shouldOverlap)
{
	if (ShouldBlock(layerId, otherLayer) && shouldBlock && ShouldOverlap(layerId, otherLayer) && shouldOverlap)
		return;

	PhysicsLayer& layerInfo = GetLayer(layerId);
	PhysicsLayer& otherLayerInfo = GetLayer(otherLayer);

	if (shouldBlock)
	{
		layerInfo.Blocking |= otherLayerInfo.BitValue;
		otherLayerInfo.Blocking |= layerInfo.BitValue;
	}
	else
	{
		layerInfo.Blocking &= ~otherLayerInfo.BitValue;
		otherLayerInfo.Blocking &= ~layerInfo.BitValue;
	}

	if (shouldOverlap)
	{
		layerInfo.Overlapping |= otherLayerInfo.BitValue;
		otherLayerInfo.Overlapping |= layerInfo.BitValue;
	}
	else
	{
		layerInfo.Overlapping &= ~otherLayerInfo.BitValue;
		otherLayerInfo.Overlapping &= ~layerInfo.BitValue;
	}
}

std::vector<PhysicsLayer> PhysicsLayerManager::GetBlockingLayers(uint layerId)
{
	const PhysicsLayer& layer = GetLayer(layerId);

	std::vector<PhysicsLayer> layers;
	for (const auto& otherLayer : m_Layers)
	{
		if (otherLayer.LayerID == layerId)
			continue;

		if(ShouldBlock(layerId, otherLayer.LayerID))
			layers.push_back(otherLayer);
	}

	return layers;
}


std::vector<PhysicsLayer> PhysicsLayerManager::GetOverlappingLayers(uint layerId)
{
	const PhysicsLayer& layer = GetLayer(layerId);

	std::vector<PhysicsLayer> layers;
	for (const auto& otherLayer : m_Layers)
	{
		if (otherLayer.LayerID == layerId)
			continue;

		if (ShouldOverlap(layerId, otherLayer.LayerID))
			layers.push_back(otherLayer);
	}

	return layers;
}

const std::vector<std::string>& PhysicsLayerManager::GetLayerNames()
{
	//because its static we gotta check each call
	if (s_LayerNames.size() < 1)
	{
		//do file stuff
		if (File::DoesFileExist(Paths::GetConfigDirectory() + "Physics.Config"))
		{
			Buffer FileBuf;
			File::ReadFile(Paths::GetConfigDirectory() + "Physics.Config", FileBuf);
			std::string stringValue = std::string(FileBuf.begin(), FileBuf.end());
			Json::Reader reader;
			Json::Value value;
			reader.parse(stringValue, value);

			for (uint i = 0; i < value["LayerNames"].size(); i++)
			{
				s_LayerNames.push_back(value["LayerNames"][i].asString());
			}
		}
		else
		{
			Json::Value value;
			for (uint i = 0; i < 10; i++)
			{
				value["LayerNames"][i] = "LayerName";
			}
			std::string stringValue = value.toStyledString();

			Buffer FileBuf = Buffer(stringValue.begin(), stringValue.end());
			File::WriteFile(Paths::GetConfigDirectory() + "Physics.Config", FileBuf);

			GetLayerNames(); // do again but we got a file this time
		}
	}

	return s_LayerNames;
}

PhysicsLayer& PhysicsLayerManager::GetLayer(uint layerId)
{
	return layerId >= m_Layers.size() ? s_NullLayer : m_Layers[layerId];
}

PhysicsLayer& PhysicsLayerManager::GetLayer(const std::string& layerName)
{
	for (auto& layer : m_Layers)
	{
		if (layer.Name == layerName)
		{
			return layer;
		}
	}

	return s_NullLayer;
}

bool PhysicsLayerManager::ShouldBlock(uint layer1, uint layer2)
{
	return GetLayer(layer1).Blocking & GetLayer(layer2).BitValue;
}

bool PhysicsLayerManager::ShouldOverlap(uint layer1, uint layer2)
{
	return GetLayer(layer1).Overlapping & GetLayer(layer2).BitValue;
}

bool PhysicsLayerManager::IsLayerValid(uint layerId)
{
	const PhysicsLayer& layer = GetLayer(layerId);
	return layer.LayerID != s_NullLayer.LayerID && layer.IsValid();
}

uint32_t PhysicsLayerManager::GetNextLayerID()
{
	int lastId = -1;

	for (const auto& layer : m_Layers)
	{
		if (lastId != -1)
		{
			if (layer.LayerID != lastId + 1)
			{
				return lastId + 1;
			}
		}

		lastId = layer.LayerID;
	}

	return (uint)m_Layers.size();
}

std::vector<std::string> PhysicsLayerManager::s_LayerNames;

