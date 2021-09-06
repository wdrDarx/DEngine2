#shader vertex
#version 460 core

#define MAX_QUADS 100000

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_ViewProjectionMatrix;

struct ST_QuadData
{
	mat4 Matrix;
	vec4 Color;
	vec3 TextureIndex; //actually just a float in X
};

layout(std430, binding = 0) buffer shader_data
{
	ST_QuadData QuadData[MAX_QUADS];
};

void main()
{		
	int QuadIndex = gl_InstanceID;
	v_TexCoord = a_TexCoords;
	
	v_Color = QuadData[QuadIndex].Color;
	v_TexIndex = QuadData[QuadIndex].TextureIndex.x;
	mat4 Transform = QuadData[QuadIndex].Matrix;
	gl_Position = u_ViewProjectionMatrix * Transform * vec4(a_Position.x, a_Position.y, a_Position.z , 1.0f);
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Textures[32];

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

void main()
{	
	int index = int(floor(v_TexIndex + 0.1));
	color = texture(u_Textures[index], v_TexCoord) * v_Color;
}