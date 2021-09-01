#pragma once
#include "Core/Core.h"

struct PhysicsLayer
{
	uint LayerID;
	std::string Name;
	uint BitValue;
	int CollidesWith = 0;

	bool IsValid() const
	{
		return !Name.empty() && BitValue > 0;
	}
};

class DENGINE_API PhysicsLayerManager
{
public:
	uint AddLayer(const std::string& name, bool setCollisions = true);
	void RemoveLayer(uint layerId);

	void SetLayerCollision(uint32_t layerId, uint32_t otherLayer, bool shouldCollide);
	std::vector<PhysicsLayer> GetLayerCollisions(uint layerId);

	const std::vector<PhysicsLayer>& GetLayers() 
	{
		return s_Layers; 
	}

	const std::vector<std::string>& GetLayerNames()
	{
		return s_LayerNames; 
	}

	PhysicsLayer& GetLayer(uint32_t layerId);
	PhysicsLayer& GetLayer(const std::string& layerName);

	uint GetLayerCount() 
	{ 
		return uint(s_Layers.size());
	}

	bool ShouldCollide(uint layer1, uint layer2);
	bool IsLayerValid(uint layerId);

private:
	uint GetNextLayerID();

private:
	std::vector<PhysicsLayer> s_Layers;
	std::vector<std::string> s_LayerNames;
	PhysicsLayer s_NullLayer = { 0, "NULL", 0, -1 };
};

