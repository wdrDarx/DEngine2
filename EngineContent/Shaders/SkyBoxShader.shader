#shader vertex
#version 460 core
layout (location = 0) in vec3 a_Pos;

out vec3 localPos;

uniform mat4 u_View;
uniform mat4 u_Projection;


void main()
{
    localPos = a_Pos;  
	
    mat4 rotView = mat4(mat3(u_View));
	vec4 clipPos = u_Projection * rotView * vec4(localPos, 1.0);

	gl_Position = clipPos.xyww;
}

#shader fragment
#version 460 core
layout(location = 0) out vec4 color;

in vec3 localPos;

uniform samplerCube u_CubeMap;

const float PI = 3.14159265359;

void main()
{		
    vec3 envColor = texture(u_CubeMap, localPos).rgb;
    
    // HDR tonemap and gamma correct
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2)); 
  
    color = vec4(envColor, 1.0);
}