#pragma once
#include "DEngine.h"

class EditorApp;
class AssetEditor
{
public:
	AssetEditor(const std::string& TargetAssetType) : m_TargetAssetType(TargetAssetType)
	{

	}

	virtual void Render() {};

	//call after setting the editor app
	virtual void Init() {};

	std::string m_TargetAssetType;
	Ref<AssetHandle> m_TargetAsset;
	EditorApp* m_App;
};