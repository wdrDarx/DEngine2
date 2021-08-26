#include "ContentBrowser.h"
#include "EditorApp.h"

void ContentBrowser::Init()
{
	//folder icon
	std::string FoldericonPath = Paths::GetEngineDirectory() + "Resources\\folder.png";
	if (File::DoesFileExist(FoldericonPath))
		m_FolderIcon = MakeRef<Texture>(Image(FoldericonPath));
 	else
		m_FolderIcon = MakeRef<Texture>(TextureSpec());

	//file icon
	std::string FileIconPath = Paths::GetEngineDirectory() + "Resources\\file.png";
	if (File::DoesFileExist(FileIconPath))
		m_FileIcon = MakeRef<Texture>(Image(FileIconPath));
 	else
 		m_FileIcon = MakeRef<Texture>(TextureSpec());
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

	uint AssetIndex = 0;

	//Asset creation menu
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !ImGui::IsPopupOpen("FileOptions"))
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

		ImGui::PushID(AssetIndex++); //AssetID

		Ref<Texture> icon = directory.is_directory() ? m_FolderIcon : m_FileIcon;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0,0,0,0});
		ImGui::ImageButton((ImTextureID)icon->GetBinding(), { m_ThumbnailSize,m_ThumbnailSize }, {0,1}, {1,0});
		ImGui::PopStyleColor();

		if (!directory.is_directory() && !m_IsDragging)
		{
			bool dragging = false;
			if (dragging = ImGui::BeginDragDropSource())
			{
				m_IsDragging = true;

				std::string payloadTag = "Asset_" + File::GetFileExtenstionFromPath(path.string());
				std::string FullPath = path.string();
				std::string TagName = File::GetFileNameFromPath(FullPath) + "." + File::GetFileExtenstionFromPath(FullPath);

				ImGui::SetDragDropPayload(payloadTag.c_str(), FullPath.c_str(), FullPath.size() + 1); // + 1 for the null terminator
				ImGui::TextUnformatted(TagName.c_str());
				ImGui::EndDragDropSource();
			}
		}

		if ( ImGui::IsItemHovered())
		{
			if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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
		}

		if (ImGui::BeginPopup("FileOptions"))
		{
			if (ImGui::MenuItem("Delete Asset"))
			{
				std::filesystem::remove(path);
				m_FlagRediscover = true;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (directory.is_regular_file() && ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("FileOptions");
		}

		std::string RenameString;
		//is the file selected, is F2 pressed and is no other file being renamed right now?
		if (directory.is_regular_file() && ImGui::IsItemHovered() && m_App->GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_F2) && m_RenamingPath.empty())
		{
			//set the renaming path to the currently selected file
			m_RenamingPath = path.string();
			RenameString = File::GetFileNameFromPath(path.string());
		}
		if (m_RenamingPath == path.string()) //is this file being renamed
		{
			ImGui::InputText("##rename", &RenameString);
			if (m_App->GetWindow()->GetInputManager().IsKeyDown(GLFW_KEY_ENTER))
			{ 
				//complete renaming
				m_RenamingPath.clear();
				std::string renamePath = path.parent_path().string() + "\\" + RenameString + path.extension().string();
				std::filesystem::rename(path, std::filesystem::path(renamePath));
			}
		}

		if(m_RenamingPath != path.string())
			ImGui::TextWrapped(filenameString.c_str());

		ImGui::PopID(); //AssetID

		ImGui::NextColumn();
	}

	m_IsDragging = false;

	ImGui::Columns(1);


	ImGui::End();
}
