#shader vertex
#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoords;

struct ST_MeshData
{
	mat4 Matrix;
};

layout(std140, binding = 0) buffer shader_data
{
	ST_MeshData s_MeshData[];
};

uniform mat4 u_ViewProjectionMatrix;


void main()
{
	int index = gl_InstanceID;
	mat4 Matrix = s_MeshData[index].Matrix;
	vec4 Pos = Matrix * vec4(a_Position, 1.0);
	gl_Position = u_ViewProjectionMatrix * Pos;
}

#shader fragment
#version 460 core

void main()
{
	//nothing
}
