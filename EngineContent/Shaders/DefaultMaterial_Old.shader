#shader vertex
#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

struct ST_MeshData
{
    mat4 Matrix;
};

layout(std430, binding = 0) buffer shader_data
{
    ST_MeshData s_MeshData[];
};

uniform mat4 u_ViewProjectionMatrix;
uniform vec4 u_Color;
uniform float u_MeshDataIndex;

out vec2 TexCoord;
out vec4 Color;

void main()
{
    int index = int(floor(u_MeshDataIndex + 0.1));
    TexCoord = a_TexCoords;
    Color = u_Color;
    mat4 Matrix = s_MeshData[index].Matrix;
    gl_Position = u_ViewProjectionMatrix * Matrix * vec4(a_Position, 1.0);
}

#shader fragment
#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec4 Color;

void main()
{
    FragColor = Color;
}