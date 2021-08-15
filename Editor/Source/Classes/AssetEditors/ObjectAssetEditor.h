#pragma once
#include "Classes/AssetEditor.h"
#include "Classes/EditorApp.h"
#include "DEngine.h"

class ObjectAssetEditor : public AssetEditor
{
public:
	ObjectAssetEditor();

	void Render() override;
	void Init() override;

	void UpdateObjectClass();

	PropertyWindow m_PropertyWindow;
	Ref<ObjectAsset> m_ObjectAsset;
	Ref<ObjectBase> m_Object;
	std::string title;
};

