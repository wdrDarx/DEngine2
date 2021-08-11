#pragma once
#include "Event/Event.h"
#include "Assets/Asset.h"


enum class AssetEventType
{
	ASSETLOAD = 0,
	ASSETSAVE
};

struct AssetEvent : public Event
{
public:
	const AssetEventType& GetEventType() const
	{
		return m_EventType;
	}

	Ref<Asset> GetAsset()
	{
		return m_Asset;
	}

	Ref<Asset> m_Asset;
	AssetEventType m_EventType;
};