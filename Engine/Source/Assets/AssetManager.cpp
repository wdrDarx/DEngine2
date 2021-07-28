#include "AssetManager.h"

void AssetManager::DiscoverAssets()
{
	for (auto& dir : GetMountedDirectories())
	{
		//this set makes sure duplicated asset id's are ignored
		std::set<AssetHandle> AssetHandles;
		for (const auto& file : std::filesystem::recursive_directory_iterator(dir.c_str()))
		{
			if (file.is_regular_file())
			{
				for (auto& reg : m_AssetTypeRegistry.GetRegisteredKeys())
				{
					if (reg.name == File::GetFileExtenstionFromPath(file.path().string()))
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

std::set<AssetHandle> AssetManager::GetAllDiscorveredAssets()
{
	std::set<AssetHandle> out;
	for (auto& dir : GetMountedDirectories())
	{
		const auto& assets = m_DiscoveredAssets[dir];
		out.insert(assets.begin(), assets.end());
	}
	return out;
}

std::vector<std::string>& AssetManager::GetMountedDirectories()
{
	std::vector<std::string> out;
	for (auto& dir : m_DiscoveredAssets)
	{
		out.push_back(dir.first);
	}
	m_MountedDirectoriesCache = out;
	return m_MountedDirectoriesCache;
}
