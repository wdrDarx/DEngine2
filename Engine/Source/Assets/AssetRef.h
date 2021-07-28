#pragma once
#include "Asset.h"
#include "AssetHandle.h"

/*
	houses an AssetHandle
*/
template<class T>
struct AssetRef
{
	public:
		AssetRef<T>()
		{
			m_AssetHandle = MakeRef<AssetHandle>();

			//set the asset type for reflection purposes
			ClassType ThisClassType(typeid(T));
			m_AssetType = ThisClassType.Name;
		}

		AssetRef<T>(const std::string& FullPathNoExtension) : m_FullPathNoExtension(FullPathNoExtension)
		{
			//construct asset handle with the path
			GetAssetHandle();
		}

		//this asset type must be registered first
		Ref<AssetHandle> GetAssetHandle()
		{
			if(m_AssetHandle)
				return m_AssetHandle;

			if(m_FullPathNoExtension.empty())
				return nullptr;
				//ASSERT(false);

			ClassType ThisClassType(typeid(T));

			//append extension
			std::string fullPath = m_FullPathNoExtension +  "." + ThisClassType.Name;

			//set this asset type
			m_AssetType = ThisClassType.Name;

			m_AssetHandle = MakeRef<AssetHandle>(fullPath);
			return m_AssetHandle;
		}

		//sets asset handle by copy
		void SetAssetHandle(const AssetHandle& handle)
		{
			m_AssetHandle = MakeRef<AssetHandle>(handle);
		}

		uint Serialize(Buffer& buffer) 
		{
			if (auto& handle = GetAssetHandle())
			{
				return handle->Serialize(buffer);
			}

			return 0;
		}

		uint Deserialize(Buffer& buffer)
		{
			if (auto& handle = GetAssetHandle())
			{
				return handle->Deserialize(buffer);
			}

			return 0;
		}

		const std::string& GetAssetType()
		{
			return m_AssetType;
		}

		bool IsValid() const
		{
			return m_AssetHandle->IsValid();
		}

private:
		Ref<AssetHandle> m_AssetHandle;		

		//stored asset type string derived from the typename
		std::string m_AssetType;

		//EXTENTION COMES FROM THE TEMPLATE
		std::string m_FullPathNoExtension;
};