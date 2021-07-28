#include "AssetHandle.h"

std::string AssetHandle::GetAssetPath()
{
	if (m_LastKnownPath.empty())
	{
		if (!m_AssetID)
		{
			LogWarning("Asset handle missing path and ID");
			return "";
		}

		if (!m_SearchDirectories) ASSERT(false);

		return FindAsset(*m_SearchDirectories, m_AssetID, m_AssetType);
	}

	if (File::DoesFileExist(m_LastKnownPath))
	{
		if (VALIDATE_ASSET && m_AssetID)
		{
			//just read the asset ID (if the asset id isnt the first thing in the file, this fails)
			Buffer assetBuffer;
			File::ReadFile(m_LastKnownPath, assetBuffer, sizeof(UID));

			UID readId;
			STARTREAD(assetBuffer, 0)
			READ(&readId, sizeof(UID));

			if (readId != m_AssetID)
			{
				//bruh
				LogWarning("existing asset switched ID's???");
				return "";
			}

		}
		else
			return m_LastKnownPath;
	}
	else //wasn't found so search recursively
	{
		if (!m_SearchDirectories) ASSERT(false);

		//updates last known path
		std::string foundPath = FindAsset(*m_SearchDirectories, m_AssetID, m_AssetType);
		Update(foundPath);

		//could be empty
		return foundPath;
	}
}

void AssetHandle::Update(const std::string& FullPath)
{
	if (UPDATE_MYSELF && !FullPath.empty())
	{
		m_LastKnownPath = FullPath;
		m_AssetType = File::GetFileExtenstionFromPath(FullPath);
		m_AssetName = File::GetFileNameFromPath(FullPath);

		//update ID
		Buffer assetBuffer;
		File::ReadFile(m_LastKnownPath, assetBuffer, sizeof(UID));

		UID readId;
		STARTREAD(assetBuffer, 0)
			READ(&readId, sizeof(UID));

		if (readId != m_AssetID)
		{
			m_AssetID = readId;
		}
	}
}

std::string AssetHandle::FindAsset(const std::vector<std::string>& SearchDirectories, UID& AssetID, const std::string& AssetType /*= ""*/)
{
	if (!AssetID) return "";

	for (const std::string& path : SearchDirectories)
	{
		for (const auto& file : std::filesystem::recursive_directory_iterator(path))
		{
			if (file.is_regular_file())
			{
				if (CHECK_ALL_FILES || File::GetFileExtenstionFromPath(file.path().string()) == AssetType)
				{
					const std::string& assetPath = file.path().string();

					//just read the asset ID (if the asset id isnt the first thing in the file, this fails)
					Buffer assetBuffer;
					File::ReadFile(assetPath, assetBuffer, sizeof(UID));

					UID readId;
					STARTREAD(assetBuffer, 0)
					READ(&readId, sizeof(UID));

					if (readId == AssetID)
					{
						//found
						return assetPath;
					}
				}
			}
		}
	}

	//couldnt find it
	LogWarning("Asset wasnt found in any directories!");
	return "";
}

uint AssetHandle::Serialize(Buffer& buffer) const
{
	STARTWRITE(buffer, 0);
	WRITE(&m_AssetID, sizeof(UID));
	WRITESTRING(m_AssetType);
	WRITESTRING(m_AssetName);
	WRITESTRING(m_LastKnownPath);
	STOPWRITE();
}

uint AssetHandle::Deserialize(const Buffer& buffer)
{
	STARTREAD(buffer, 0);
	READ(&m_AssetID, sizeof(UID));
	READSTRING(m_AssetType);
	READSTRING(m_AssetName);
	READSTRING(m_LastKnownPath);
	STOPREAD();
}
