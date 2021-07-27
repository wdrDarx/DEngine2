#include "Asset.h"

Asset::Asset()
{
	//assign random id on construct (will get overriden on deserialize)
	m_ID.ID = Rand::Int64();
}
