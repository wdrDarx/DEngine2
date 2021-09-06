#shader vertex
#version 460 core
layout (location = 0) in vec3 a_Pos;

out vec3 localPos;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    localPos = a_Pos;  
    gl_Position =  u_Projection * u_View * vec4(localPos, 1.0);
}

#shader fragment
#version 460 core
out vec4 FragColor;

in vec3 localPos;

uniform sampler2D u_CubeMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(localPos)); // make sure to normalize localPos
    vec3 tex = texture(u_CubeMap, uv).rgb;
    
    FragColor = vec4(tex, 1.0);
}
