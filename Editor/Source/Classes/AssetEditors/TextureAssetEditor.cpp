#include "TextureAssetEditor.h"

TextureAssetEditor::TextureAssetEditor() : AssetEditor("TextureAsset")
{

}

void TextureAssetEditor::Init()
{
	m_QuadRenderer = MakeRef<QuadRenderer>(m_App->m_EditorScene->GetPipeline().get());
	m_QuadRenderer->Initialize(ObjectInitializer());
	m_Viewport = MakeRef<Viewport>(nullptr, m_App->GetWindow(), "Texture View");
}

void TextureAssetEditor::Render()
{
	m_TextureAsset = m_App->GetAssetManager().LoadAsset<TextureAsset>(m_TargetAsset);

	if(!m_TextureAsset) return;

	title = m_TargetAsset->GetAssetName() + " Texture Editor";
	bool isOpen = true;
	ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoDocking);

	if (ImGui::Button("Save"))
	{
		m_App->GetAssetManager().SaveAsset(m_TextureAsset, m_TargetAsset);
	}

	ImGui::Columns(2);

	m_Viewport->BeginFrame();

	m_QuadRenderer->ClearFrame();

	Transform trans;
	trans.pos.z = -200.f;
	float ratio = (float)m_TextureAsset->m_width / (float)m_TextureAsset->m_height;
	float sizeY = 100.f / ratio;

	m_QuadRenderer->DrawQuad3D({100, sizeY}, trans, {1,1,1,1}, m_TextureAsset->GetTexture());
	m_QuadRenderer->PrepareFrame();
	m_QuadRenderer->RenderFrame(m_Viewport->m_Camera);

	m_Viewport->EndFrame(false);

	ImGui::NextColumn();

	//properties
	if (ImGui::Button("Load From file"))
	{
		Image loadedimg(FileDialog::OpenFile(""));
		m_TextureAsset = MakeRef<TextureAsset>(loadedimg);
		m_App->GetAssetManager().SaveAsset(m_TextureAsset, m_TargetAsset);
	}
	
	std::string assetText =  "Width : " + STRING(m_TextureAsset->m_width);
	ImGui::Text(assetText.c_str());

	assetText = "Height : " + STRING(m_TextureAsset->m_height);
	ImGui::Text(assetText.c_str());

	bool expanded = ImGui::TreeNodeEx((void*)("Options"), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen, "Options");
	if (expanded)
	{
		bool NearestFilter = m_TextureAsset->m_Spec.Filter == FilterType::NEAREST;
		bool Wrap = m_TextureAsset->m_Spec.Wrap == WrapMode::CLAMP;
		ImGui::Checkbox("Anisotropic", &m_TextureAsset->m_Spec.Anisotropic);
		ImGui::Checkbox("Filter By Nearest", &NearestFilter);
		ImGui::Checkbox("Clamp To Edge", &Wrap);
	
		m_TextureAsset->m_Spec.Filter = NearestFilter ?  FilterType::NEAREST : FilterType::LINEAR;
		m_TextureAsset->m_Spec.Wrap = Wrap ? WrapMode::CLAMP : WrapMode::REPEAT;

		ImGui::TreePop();
	}

	ImGui::Columns(1);

	ImGui::End();
	if (!isOpen)
	{ 
		m_App->RemoveAssetEditor(this);
	}
}


