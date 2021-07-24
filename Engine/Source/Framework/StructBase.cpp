#include "StructBase.h"

void StructBase::SerializeProps(ArrayBuffer& buffer) const
{
	for (const auto& prop : GetProperties())
	{
		buffer.AddPiece(prop.MakeBuffer());
	}
}

void StructBase::LoadProps(const ArrayBuffer& buffer)
{
	for (auto& piece : buffer.m_DataPieces)
	{
		Property NewProp;
		//need to deserialize new prop to get its name and type
		NewProp.LoadNameAndType(piece);

		//Go through old props and update them if a new one overdies it
		for (auto& oldProp : m_Properties)
		{
			//If a new prop didnt exist before then ignore it and dont laod it
			if (oldProp.m_name == NewProp.m_name && oldProp.m_Type == NewProp.m_Type)
			{
				//LogTemp("(DStruct) " + std::string("Loading Property : ") + NewProp.m_name);

				//load the old prop with the new prop data
				oldProp.FromBuffer(piece);
			}
		}
	}
}
