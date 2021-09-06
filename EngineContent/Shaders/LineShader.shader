#shader vertex
#version 460 core

struct Line
{
    vec4 start;
    vec4 end;
    vec4 color;
};

layout(std430, binding = 0) buffer shader_data
{
    Line s_Lines[];
};

uniform mat4 u_ViewProjectionMatrix;

out vec4 Color;

void main()
{
    Color = s_Lines[gl_InstanceID].color;
    vec3 pos = vec3(gl_VertexID == 0 ? s_Lines[gl_InstanceID].start : s_Lines[gl_InstanceID].end);
    gl_Position = u_ViewProjectionMatrix * vec4(pos, 1.0);
}

#shader fragment
#version 460 core
out vec4 FragColor;

in vec4 Color;

void main()
{		
    FragColor = Color;
}