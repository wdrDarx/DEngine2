#pragma once
#include "Framework/AppObject.h"
#include "Framework/FrameworkMacros.h"
#include "PhysicsSettings.h"

/*
	Global AppObject
*/
class PhysicsLayerManager;
class PhysicsScene;
class PhysXAPI;
class CookingFactory;
class DENGINE_API PhysicsWorld : public AppObject
{
	public:
		OBJECT_CLASS_DEF(PhysicsWorld, AppObject)

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnDestroy() override;

	void OnBeginPlay() override;
	void OnEndPlay() override;

	Ref<PhysicsScene> CreatePhysicsScene(const PhysicsSettings* CustomSettings = nullptr);

	const std::vector<Ref<PhysicsScene>>& GetPhysicsScene() const
	{
		return m_PhysicsScenes;
	}

	Ref<PhysicsLayerManager> GetPhysicsLayerManager()
	{
		return m_PhysicsLayerManager;
	}

	Ref<PhysXAPI> GetPhysicsAPI()
	{
		return m_API;
	}

	Ref<CookingFactory> GetCookingFactory()
	{
		return m_CookingFactory;
	}

	const PhysicsSettings& GetPhysicsSettings() const
	{
		return m_PhysicsSettings;
	}

	void SetPhysicsSettings(const PhysicsSettings& settings)
	{
		m_PhysicsSettings = settings;
	}

public:
	PhysicsSettings m_PhysicsSettings;
	Ref<PhysicsLayerManager> m_PhysicsLayerManager;

	std::vector<Ref<PhysicsScene>> m_PhysicsScenes;

	Ref<PhysXAPI> m_API;
	Ref<CookingFactory> m_CookingFactory;
};

