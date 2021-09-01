#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Utils/Task.h"

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
	void DrawDebugCube(const vec3d& pos, const vec3d& rot, const vec3d& size, const color3& color);

private:

	std::vector<DebugCube> m_CubeBuffer;
	std::vector<LineSegment> m_LineBuffer;
	Ref<ShaderStorageBuffer> m_StorageBuffer;
};


