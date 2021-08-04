#include "ContentBrowser.h"
#include "EditorApp.h"

void ContentBrowser::Init()
{
	//folder icon
	std::string FoldericonPath = Paths::GetEngineDirectory() + "Resources\\folder.png";
	if (File::DoesFileExist(FoldericonPath))
		m_FolderIcon = MakeRef<Texture>(FoldericonPath);
 	else
		m_FolderIcon = MakeRef<Texture>();

	//file icon
	std::string FileIconPath = Paths::GetEngineDirectory() + "Resources\\file.png";
	if (File::DoesFileExist(FileIconPath))
		m_FileIcon = MakeRef<Texture>(FileIconPath);
 	else
 		m_FileIcon = MakeRef<Texture>();
}

void ContentBrowser::Render(EditorApp* m_App)
{
	//discover assets
	if(m_FlagRediscover)
	{ 
		m_App->GetAssetManager().DiscoverAssets();
		m_FlagRediscover = false;
	}

	ImGui::Begin("Content Browser");
	std::filesystem::path ContentPath = Paths::GetContentDirectory();

	//reset path if invalid
	if(!std::filesystem::exists(m_CurrentPath))
		m_CurrentPath = Paths::GetContentDirectory();

	//back button
	if (m_CurrentPath != ContentPath)
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
		}
	}

	float CellSize = m_ThumbnailSize + m_Padding;
	float PanelWidth = ImGui::GetContentRegionAvailWidth();
	uint columnCount = (uint)(PanelWidth / CellSize);
	if(columnCount < 1)
		columnCount = 1;
	ImGui::Columns(columnCount, 0, false);

	for (auto& directory : std::filesystem::directory_iterator(m_CurrentPath))
	{
		//if its a file, check if the asset type is registered, otherwise skip it
		bool ShouldSkip = false;
		if (!directory.is_directory())
		{
			bool found = false;
			for (auto& key : m_App->GetAssetManager().GetAssetTypeRegistry().GetRegisteredKeys())
			{
				std::string extension = File::GetFileExtenstionFromPath(directory.path().string());

				if(extension == key.name)
					found = true;
			}
			if(!found)
				ShouldSkip = true;
		}
		if(ShouldSkip)
			continue;

		auto& path = directory.path();
		auto relativePath = std::filesystem::relative(path, ContentPath);
		std::string filenameString = relativePath.filename().string();

		Ref<Texture> icon = directory.is_directory() ? m_FolderIcon : m_FileIcon;
		ImGui::ImageButton((ImTextureID)icon->m_RendererID, { m_ThumbnailSize,m_ThumbnailSize }, {0,1}, {1,0});
		if ( ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directory.is_directory())
			{
				m_CurrentPath /= path.filename();
			}
			else
			{
				Ref<AssetHandle> assetHandle = MakeRef<AssetHandle>(directory.path().string());
				m_App->AddAssetEditor(assetHandle);
			}
		}
		ImGui::TextWrapped(filenameString.c_str());


		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	//Asset creation menu
	if(ImGui::IsMouseClicked(ImGuiMouseButton_Right))
	{
		ImGui::OpenPopup("New");	
	}

	if (ImGui::BeginPopup("New"))
	{
		for (auto& key : m_App->GetAssetManager().GetAssetTypeRegistry().GetRegisteredKeys())
		{
			std::string ItemText = "New " + key.name;
			if (ImGui::MenuItem(ItemText.c_str()))
			{				
				Ref<Asset> newAsset = ToRef<Asset>(m_App->GetAssetManager().GetAssetTypeRegistry().Make(key));
				m_App->GetAssetManager().SaveAsset(newAsset, m_CurrentPath.string() + "\\Asset_" + STRING(newAsset->GetID().ID) + "." + key.name);
				m_FlagRediscover = true;
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
	ImGui::End();
}
