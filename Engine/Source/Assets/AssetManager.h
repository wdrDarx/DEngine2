#pragma once
#include "Core/Core.h"
#include "Utils/DataCache.h"
#include "Utils/Paths.h"
#include "Utils/FileSystem.h"
#include "Serialization/Buffer.h"
#include "Framework/Registry.h"
#include "Asset.h"
#include "AssetRef.h"

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
	template<class T>
	void SaveAsset(Ref<T> asset, const std::string& FullPath)
	{
		if (!std::is_base_of<Asset, T>::value)
		{
			ASSERT(false);
		}

		Buffer buffer;
		asset->Serialize(buffer);

		//update the cache since we know the asset changed
		m_AssetCache->Set(FullPath, asset);

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
		//set the search directories
		assetRef.GetAssetHandle()->m_SearchDirectories = &GetMountedDirectories();

		//actually load the asset
		return LoadAsset<T>(assetRef.GetAssetHandle()->GetAssetPath());
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

		return newAsset;
	}

	//goes through and discovers any registered asset types in all mounted directories (ASSETS WITH THE SAME ASSET ID WILL BE IGNORED AND NOT INCLUDED HERE)
	void DiscoverAssets()
	{
		for(auto& dir : GetMountedDirectories())
		{ 
			//this set makes sure duplicated asset id's are ignored
			std::set<AssetHandle> AssetHandles;
			for (const auto& file : std::filesystem::recursive_directory_iterator(dir.c_str()))
			{
				if (file.is_regular_file())
				{
					for (auto& reg : m_AssetTypeRegistry.GetRegisteredKeys())
					{
						if(reg.name == File::GetFileExtenstion(file.path().string()))
						{
							AssetHandles.insert(AssetHandle(file.path().string()));
							break;
						}
					}
				}
			}

			//replace the directory assets with this new set
			m_DiscoveredAssets[dir] = AssetHandles;
		}
	}

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

	//appends all asset handles from all mounted directories into 1 set (ASSETS WITH THE SAME ASSET ID WILL BE IGNORED AND NOT INCLUDED HERE)
	std::set<AssetHandle> GetAllDiscorveredAssets() 
	{
		std::set<AssetHandle> out;
		for (auto& dir : GetMountedDirectories())
		{
			const auto& assets = m_DiscoveredAssets[dir];
			out.insert(assets.begin(), assets.end());
		}
		return out;
	}

	std::vector<std::string>& GetMountedDirectories()
	{
		std::vector<std::string> out;
		for (auto& dir : m_DiscoveredAssets)
		{
			out.push_back(dir.first);
		}
		m_MountedDirectoriesCache = out;
		return m_MountedDirectoriesCache;
	}

private:
	//the tag is the full path to the asset (extension included)
	Ref<DataCache> m_AssetCache;
	 
	//list of available asset handles (key = Base Directory, Value = set of asset handles)
	std::unordered_map<std::string, std::set<AssetHandle>> m_DiscoveredAssets;

	std::vector<std::string> m_MountedDirectoriesCache;

	AssetRegistry m_AssetTypeRegistry;
};

