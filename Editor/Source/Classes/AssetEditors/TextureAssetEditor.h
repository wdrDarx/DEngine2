#pragma once
#include "Classes/AssetEditor.h"
#include "Classes/EditorApp.h"
#include "DEngine.h"

class TextureAssetEditor : public AssetEditor
{
public:
	TextureAssetEditor();

	void Render() override;
	void Init() override;

	Ref<TextureAsset> m_TextureAsset;
	std::string title;
	Ref<Viewport> m_Viewport;
	Ref<QuadRenderer> m_QuadRenderer;
	

};

