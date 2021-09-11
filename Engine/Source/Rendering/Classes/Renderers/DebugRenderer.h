#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

struct DENGINE_API RenderFlags : public EnumBase
{
	enum Enum
	{
		DEBUGLINES = BIT(1),
		DEBUGCUBES = BIT(2),
		PHYSX = BIT(3),
		GRID = BIT(4),
	};

	BITMASK_DEF_BEGIN(RenderFlags, Enum)
		ENUM_DEF(DEBUGLINES);
		ENUM_DEF(DEBUGCUBES);
		ENUM_DEF(PHYSX);
		ENUM_DEF(GRID);
	ENUM_DEF_END();
};

struct DENGINE_API DebugMesh
{
	Ref<Mesh> mesh;
	Transform trans;
	color3 color;
};

struct DENGINE_API DebugCube
{
	Transform trans;
	color3 color;
};

struct DENGINE_API LineSegment
{
	vec4d start;
	vec4d end;
	color4 color;
};

/*
	Renderer for debug stuff like lines, wireframe and shapes
*/
class DENGINE_API DebugRenderer : public Renderer
{
public:
	OBJECT_CLASS_DEF(DebugRenderer, Renderer)

	void OnConstruct() override;

	void PrepareFrame() override;
	void RenderFrame(Ref<Camera> camera) override;
	void ClearFrame() override;

	void DrawDebugLine(const vec3d& start, const vec3d& end, const color3& color);
	void DrawDebugLineStrip(const std::vector<vec3d>& positions, const color3& color);

	void DrawDebugLine(const vec3d& start, const vec3d& end, const color4& color);
	void DrawDebugLineStrip(const std::vector<vec3d>& positions, const color4& color);

	void DrawDebugCube(const vec3d& pos, const vec3d& rot, const vec3d& size, const color3& color);

	void DrawGrid(float DivisionSize, float TotalSize, const color4& color = {0.7f, 0.7f, 0.7f, 0.3f});

	void SetRenderFlags(const RenderFlags& flags)
	{
		m_RenderFlags = flags;
	}

	const RenderFlags& GetRenderFlags()
	{
		return m_RenderFlags;
	}

	float m_LineWidth = 1.0f;
private:

	RenderFlags m_RenderFlags = 0;
	std::vector<DebugCube> m_CubeBuffer;
	std::vector<LineSegment> m_LineBuffer;
	Ref<ShaderStorageBuffer> m_StorageBuffer;
};


