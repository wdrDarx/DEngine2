#shader vertex
#version 460 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoords;

out vec2 TexCoord;

void main()
{
    TexCoord = a_TexCoords;
    gl_Position =  vec4(a_Position, 1.0);
}

#shader fragment
#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_Texture;

void main()
{		
    FragColor = vec4(texture(u_Texture, TexCoord).rgb, 1.0);
}