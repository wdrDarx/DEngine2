#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 0) in vec2 a_TexCoords;

uniform vec4 u_Color;
uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_Model;

out vec4 Color;

void main()
{		
	vec3 Pos = vec3(u_Model * vec4(a_Position, 1.0));
	Color = u_Color;

	gl_Position = u_ViewProjectionMatrix * vec4(Pos, 1.0f);
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 Color;

void main()
{		
	color = Color;
}