#shader vertex
#version 460 core

#define MAX_QUADS 100000

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in float a_TexIndex;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in float a_MatrixIndex;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_ViewProjectionMatrix;

layout(binding = 2) buffer shader_data
{
	mat4 s_Matricies[MAX_QUADS];
};

void main()
{		
	v_Color = a_Color;
	v_TexCoord = a_TexCoords;
	v_TexIndex = a_TexIndex;

	int MatrixIndex = int(floor(a_MatrixIndex + 0.1));
	mat4 Transform = s_Matricies[MatrixIndex];
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
	//color = vec4(v_TexCoord.x, v_TexCoord.y, 0, 1);
}