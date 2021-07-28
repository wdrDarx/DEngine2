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

	AssetHandle()
	{

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
	std::string GetAssetPath();

	//Path must be valid, no checking will occur here
	void Update(const std::string& FullPath);

	//you can leave asset type empty only if CHECK_ALL_FILES is enabled
	static std::string FindAsset(const std::vector<std::string>& SearchDirectories, UID& AssetID, const std::string& AssetType = "");

	uint Serialize(Buffer& buffer) const;
	uint Deserialize(const Buffer& buffer);

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

	//check to see if the asset being pointed to is valid
	bool IsValid() const
	{
		return m_AssetID != 0;
	}

public:
	//assigned by asset manager
	std::vector<std::string>* m_SearchDirectories = nullptr;
private:
	std::string m_AssetName;
	std::string m_AssetType;
	UID m_AssetID;
	std::string m_LastKnownPath;
};