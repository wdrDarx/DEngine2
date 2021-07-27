#pragma once
#include "Asset.h"
#include "Utils/FileSystem.h"
#include "Utils/Paths.h"

//opens the found file and checks the asset id and type just in case
#define VALIDATE_ASSET 0 

//checks files of all types for asset id not just the set type extension
#define CHECK_ALL_FILES 0

//whether to set m_AssetType based on the new found asset after a recursive search by ID or a path
#define UPDATE_MYSELF 1

//class for retrieving an asset in a file system
struct DENGINE_API AssetHandle
{
	bool operator<(const AssetHandle& other) const
	{
		return m_AssetID.ID < other.m_AssetID.ID;
	}

	AssetHandle(const UID& AssetID, const std::string& AssetType) : m_AssetID(AssetID), m_AssetType(AssetType)
	{

	}

	AssetHandle(const UID& AssetID, const std::string& AssetType, const std::string) : m_AssetID(AssetID), m_AssetType(AssetType)
	{

	}

	//careful with this one
	AssetHandle(const std::string& FullPotentialPath) : m_LastKnownPath(FullPotentialPath)
	{
		if(UPDATE_MYSELF)
			Update(FullPotentialPath);
	}



	/*
	try to retrieve valid path for asset being refernced
	if a file isnt found at first, it recursively looks for it and updates itself if the asset was found
	returns full path for found asset
	an empty string is returned if the asset was not found
	*/
	std::string GetAssetPath()
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

				if(readId != m_AssetID)
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
			//updates last known path
			if(!m_SearchDirectories) ASSERT(false);

			std::string foundPath = FindAsset(*m_SearchDirectories, m_AssetID, m_AssetType);
			Update(foundPath);

			//could be empty
			return foundPath;
		}
	}

	//Path must be valid, no checking will occur here
	void Update(const std::string& FullPath)
	{
		if (UPDATE_MYSELF && !FullPath.empty())
		{
			m_LastKnownPath = FullPath;
			m_AssetType = File::GetFileExtenstion(FullPath);
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

	//you can leave asset type empty only if CHECK_ALL_FILES is enabled
	static std::string FindAsset(const std::vector<std::string>& SearchDirectories, UID& AssetID, const std::string& AssetType = "")
	{
		if(!AssetID) return "";

		for (const std::string& path : SearchDirectories)
		{
			for (const auto& file : std::filesystem::recursive_directory_iterator(path))
			{
				if (file.is_regular_file())
				{
					if(CHECK_ALL_FILES || file.path().extension() == AssetType)
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

	uint Serialize(Buffer& buffer) const
	{
		STARTWRITE(buffer, 0);
		WRITE(&m_AssetID, sizeof(UID));
		WRITESTRING(m_AssetType);
		WRITESTRING(m_AssetName);
		WRITESTRING(m_LastKnownPath);
		STOPWRITE();
	}

	uint Deserialize(const Buffer& buffer)
	{
		STARTREAD(buffer, 0);
		READ(&m_AssetID, sizeof(UID));
		READSTRING(m_AssetType);
		READSTRING(m_AssetName);
		READSTRING(m_LastKnownPath);
		STOPREAD();
	}

	const std::string& GetAssetName() const
	{
		return m_AssetName;
	}

	const std::string& GetAssetType() const
	{
		return m_AssetType;
	}

	const UID& GetAssetID() const
	{
		return m_AssetID;
	}

public:
	//assigned by asset manager
	std::vector<std::string>* m_SearchDirectories = nullptr;

	std::string m_AssetName;
	std::string m_AssetType;
	UID m_AssetID;
private:
	std::string m_LastKnownPath;
};