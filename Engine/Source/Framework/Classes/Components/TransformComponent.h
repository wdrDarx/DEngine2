#pragma once
#include "Framework/ObjectComponent.h"
#include "Utils/World.h"
/*
	unique identifies for a local component within a scene object
	because component id's are generated on CreateComponent on construct
	and aren't serialized, we gotta use the names as Id's which sucks ):
*/
struct LocalComponentRef
{
	std::string ComponentName;

	bool operator==(const LocalComponentRef& other) const
	{
		return ComponentName == other.ComponentName;
	}

	bool operator!=(const LocalComponentRef& other) const
	{
		return ComponentName != other.ComponentName;
	}

	operator bool() const
	{
		return !ComponentName.empty();
	}

	void Serialize(Buffer& buffer) const
	{
		STARTWRITE(buffer, 0);
		WRITESTRING(ComponentName);
	}

	void Deserialize(const Buffer& buffer)
	{
		STARTREAD(buffer, 0);
		READSTRING(ComponentName);
	}
};


class DENGINE_API TransformComponent : public ObjectComponent
{
public:
	OBJECT_CLASS_DEF(TransformComponent, ObjectComponent);
	OBJECT_PROPS_BEGIN()
		PROPDEF(m_Transform, EditAnywhere);
	OBJECT_PROPS_END()
		
	//used to load the parent component if this is after being deserialized
	void OnPostConstruct() override;

	//used to detect changes in transform to clear cache
	void OnUpdate(const Tick& tick) override;

	const Transform& GetLocalTransform() const
	{
		return m_Transform;
	}

	void SetLocalTransform(const Transform& LocalTrans)
	{
		if(LocalTrans != m_Transform)
		{ 
			m_Transform = LocalTrans;

			//clear caches
			ClearCache();
		}
	}

	//recursively gets world matrix by combining transforms of all the parents on the hierarchy
	const glm::mat4& GetWorldMatrix();

	Transform GetWorldTransform()
	{
		if (!m_WorldTransformCache)
		{
			m_WorldTransformCache = World::MakeTransform(GetWorldMatrix());
		}

		return m_WorldTransformCache.value();
	}

	void SetWorldTransform(const Transform& WorldTrans)
	{
		if (m_WorldTransformCache)
		{
			if(m_WorldTransformCache == WorldTrans) return;
		}

		SetWorldMatrix(World::MakeMatrix(WorldTrans));
	}

	//idk if is even correct really
	void SetWorldMatrix(const glm::mat4& matrix)
	{
		SetMatrix(glm::inverse(GetWorldMatrix()) * matrix);
	}

	const glm::mat4& GetMatrix()
	{
		if(!m_LocalMatrixCache)
			m_LocalMatrixCache = World::MakeMatrix(m_Transform);

		return m_LocalMatrixCache.value();
	}

	void SetMatrix(const glm::mat4& mat)
	{
		if (m_LocalMatrixCache)
		{
			if (m_LocalMatrixCache == mat) return;
		}
		m_LocalMatrixCache = mat;

		SetLocalTransform(World::MakeTransform(mat));
	}

	//Position
	vec3d GetWorldPostition()
	{
		return GetWorldTransform().pos;
	}

	void SetLocalPosition(const vec3d& pos)
	{
		Transform tr = GetLocalTransform();
		SetLocalTransform({ pos, tr.rot, tr.scale });
	}

	void SetWorldPosition(const vec3d& pos)
	{
		Transform tr = GetWorldTransform();
		SetWorldTransform({ pos, tr.rot, tr.scale });
	}

	//rotation
	const vec3d& GetWorldRotation()
	{
		return GetWorldTransform().rot;
	}

	void SetLocalRotation(const vec3d& rot)
	{
		Transform tr = GetLocalTransform();
		SetLocalTransform({ tr.pos, rot, tr.scale });
	}

	void SetWorldRotation(const vec3d& rot)
	{
		Transform tr = GetWorldTransform();
		SetWorldTransform({ tr.pos, rot, tr.scale });
	}

	//scale
	const vec3d& GetLocalScale()
	{
		return GetLocalTransform().scale;
	}

	void SetLocalScale(const vec3d& scale)
	{
		Transform tr = GetLocalTransform();
		SetLocalTransform({ tr.pos, tr.rot, scale });
	}

	void SetWorldScale(const vec3d& scale)
	{
		Transform tr = GetWorldTransform();
		SetWorldTransform({ tr.pos, tr.rot, scale });
	}

	//offsets
	void AddWorldTransform(const Transform& trans)
	{
		SetWorldTransform(GetWorldTransform() + trans);
	}

	void AddWorldPostionOffset(const vec3d& pos)
	{
		AddWorldTransform({ pos, {0,0,0}, {0,0,0} });
	}

	void AddWorldRotationOffset(const vec3d& rot)
	{
		AddWorldTransform({ {0,0,0}, rot, {0,0,0} });
	}

	Ref<TransformComponent> GetParent() const
	{
		return m_Parent;
	}

	void AttachTo(Ref<TransformComponent> NewParent)
	{
		ASSERT(NewParent); //cant be nullptr, use Detach to clear parent
		if(NewParent.get() != this)
		{ 
			m_Parent = NewParent;

			//clear caches
			ClearCache();
		}
	}

	void Detach()
	{
		m_Parent = nullptr;

		//clear caches
		ClearCache();
	}


	void ClearCache()
	{
		//clear mine
		m_LocalMatrixCache = {};
		m_WorldTransformCache = {};

		//clear for all childred
		for (auto& child : GetChildren())
		{
			child->ClearCache();
		}
	}

	bool IsRootComponent() const;

	//needs to have an owner, children are got by scanning all components and checking if they're attached to us
	std::vector<Ref<TransformComponent>> GetChildren() const;

	//override to serialize the parent reference
	uint Serialize(Buffer& buffer) const override;

	//override to load in parent component and attach to it
	uint Deserialize(const Buffer& buffer) override;
		
private:
	//represents the local space transform
	Transform m_Transform;

	//used to detect transform changed to clear cache
	Transform m_LastTransform;
	
	//ref to the parent, could be null 
	//auto set on deserialize if there was a parent
	Ref<TransformComponent> m_Parent;

	//for serialization of the parent component
	LocalComponentRef m_ParentRef;

	//local matrix, updates when local transform is changed
	std::optional<glm::mat4> m_LocalMatrixCache;

	//world space transform, updated when the parent's world matrix is changed or parent is switched
	std::optional<Transform> m_WorldTransformCache;

};