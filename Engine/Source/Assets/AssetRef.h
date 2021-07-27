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

		}

		AssetRef<T>(const std::string& FullPathNoExtension) : m_FullPathNoExtension(FullPathNoExtension)
		{

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
		}

		uint Deserialize(Buffer& buffer)
		{
			if (auto& handle = GetAssetHandle())
			{
				return handle->Deserialize(buffer);
			}
		}

private:
		Ref<AssetHandle> m_AssetHandle;		

		//EXTENTION COMES FROM THE TEMPLATE
		std::string m_FullPathNoExtension;
};