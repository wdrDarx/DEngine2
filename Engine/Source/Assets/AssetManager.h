#pragma once
#include "Core/Core.h"
#include "Utils/DataCache.h"
#include "Utils/Paths.h"
#include "Utils/FileSystem.h"
#include "Serialization/Buffer.h"
#include "Framework/Registry.h"
#include "Asset.h"
#include "AssetRef.h"
#include "Event/AssetEvent.h"
#include "Event/EventDispatcher.h"
#include "Event/Callback.h"

class DENGINE_API AssetManager
{
public:
	AssetManager()
	{
		m_AssetCache = MakeRef<DataCache>();
	}


	template<class T>
	void SaveAsset(Ref<T> asset, Ref<AssetHandle> handle)
	{
		if (!std::is_base_of<Asset, T>::value)
		{
			ASSERT(false);
		}

		SaveAsset(asset, handle->GetAssetPath());
	}


	//This is an internal function, you should use an AssetRef<> instead
	void SaveAsset(Ref<Asset> asset, const std::string& FullPath)
	{
		Buffer buffer;
		asset->Serialize(buffer);

		//update the cache since we know the asset changed
		m_AssetCache->Set(FullPath, asset);

		//Asset Load Event
		AssetEvent event;
		event.m_Asset = asset;
		event.m_EventType = AssetEventType::ASSETSAVE;
		GetEventDispatcher().Dispatch(event);

		LogTemp("Saving Asset : " + FullPath);
		File::WriteFile(FullPath, buffer);
	}

	void UnloadAll()
	{
		m_AssetCache->Clear();
	}

	void Unload(const std::string& FullPath)
	{
		m_AssetCache->Remove(FullPath);
	}

	//get extension from type (this also just checks if the asset is registered)
	template<class T> 
	std::string GetAssetExtenstion()
	{	
		if (!std::is_base_of<Asset, T>::value)
		{
			ASSERT(false);
		}

		std::string extension;
		ClassType AssetClassType(typeid(T));
		for (const auto& a : m_AssetTypeRegistry.GetRegisteredKeys())
		{
			if (a.name == AssetClassType.Name)
				extension = a.name;
		}
		if (extension.empty())
		{
			LogWarning("Failed to extension - Asset type not registered!");
		}

		return extension;
	}

	//auto casts the loaded asset
	template<class T>
	Ref<T> LoadAsset(AssetRef<T>& assetRef)
	{		
		if (!assetRef.GetAssetHandle()) return nullptr;
		if(!assetRef.IsValid()) return nullptr;

		//set the search directories
		assetRef.GetAssetHandle()->m_SearchDirectories = &GetMountedDirectories();

		//actually load the asset	
		return LoadAsset<T>(assetRef.GetAssetHandle()->GetAssetPath());
	}

	//this is only if you're sure the asset pointer to is the correct type and stuff
	template<class T>
	Ref<T> LoadAsset(Ref<AssetHandle> assetHandle)
	{
		if (!assetHandle) return nullptr;
		if (!assetHandle->IsValid()) return nullptr;

		//set the search directories
		assetHandle->m_SearchDirectories = &GetMountedDirectories();

		//actually load the asset	
		return LoadAsset<T>(assetHandle->GetAssetPath());
	}

	//This is an internal function, you should use an AssetRef<> instead
	template<class T>
	Ref<T> LoadAsset(const std::string& FullPath)
	{
		if (!std::is_base_of<Asset, T>::value)
		{
			LogWarning("Failed to load Asset! - Type is not an asset");
			return nullptr;
		}

		//check if asset is already loaded
		Ref<T> out = StaticCast<T>(m_AssetCache->Get(FullPath));
		if (out)
			return out;

		//Asset not loaded so load it
		if (!File::DoesFileExist(FullPath))
		{
			LogWarning("Failed to load Asset! - asset not found in directory");
			return nullptr;
		}

		Buffer buffer;
		LogTemp("Loading Asset : " + FullPath);
		File::ReadFile(FullPath, buffer);

		Ref<T> newAsset = MakeRef<T>();
		newAsset->Deserialize(buffer);

		//add to cache
		m_AssetCache->Set(FullPath, newAsset);

		//Asset Load Event
		AssetEvent event;
		event.m_Asset = newAsset;
		event.m_EventType = AssetEventType::ASSETLOAD;
		GetEventDispatcher().Dispatch(event);

		return newAsset;
	}

	//goes through and discovers any registered asset types in all mounted directories (ASSETS WITH THE SAME ASSET ID WILL BE IGNORED AND NOT INCLUDED HERE)
	void DiscoverAssets();

	void MountContentDirectory(const std::string& FullPath)
	{
		//creates an empty value
		m_DiscoveredAssets[FullPath];
	}

	void UnmountContentDirectory(const std::string& FullPath)
	{
		m_DiscoveredAssets.erase(FullPath);
	}
	
	AssetRegistry& GetAssetTypeRegistry()
	{
		return m_AssetTypeRegistry;
	}

	const std::set<AssetHandle>& GetDiscorveredAssets(const std::string& BaseDir)
	{
		return m_DiscoveredAssets[BaseDir];
	}

	EventDispatcher& GetEventDispatcher()
	{
		return m_AssetEventDispatcher;
	}

	//appends all asset handles from all mounted directories into 1 set (ASSETS WITH THE SAME ASSET ID WILL BE IGNORED AND NOT INCLUDED HERE)
	std::set<AssetHandle> GetAllDiscorveredAssets();
	std::vector<std::string>& GetMountedDirectories();

private:
	//the tag is the full path to the asset (extension included)
	Ref<DataCache> m_AssetCache;
	 
	//list of available asset handles (key = Base Directory, Value = set of asset handles)
	std::unordered_map<std::string, std::set<AssetHandle>> m_DiscoveredAssets;

	//list of directories the manager is aware of when discovering assets
	std::vector<std::string> m_MountedDirectoriesCache;

	//stores asset types in a factory (key is the class name)
	AssetRegistry m_AssetTypeRegistry;

	//sends AssetEvent
	EventDispatcher m_AssetEventDispatcher;
};

