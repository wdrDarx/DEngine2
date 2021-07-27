#shader vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in float a_TexIndex;
layout(location = 3) in vec4 a_Color;
layout(location = 4) in mat4 a_Transform;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;

uniform mat4 u_ViewProjectionMatrix;

void main()
{		
	v_Color = a_Color;
	v_TexCoord = a_TexCoords;
	v_TexIndex = a_TexIndex;
	gl_Position = u_ViewProjectionMatrix * a_Transform * vec4(a_Position.x, a_Position.y, a_Position.z , 1.0f);
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;

void main()
{	
	color = v_Color * texture(v_TexIndex, v_TexCoord);
	//color = vec4(1.0);
}