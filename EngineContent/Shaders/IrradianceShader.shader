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

uniform samplerCube u_CubeMap;

const float PI = 3.14159265359;

void main()
{		
   vec3 normal = normalize(localPos);
  vec3 irradiance = vec3(0.0);  

vec3 up    = vec3(0.0, 1.0, 0.0);
vec3 right = normalize(cross(up, normal));
up         = normalize(cross(normal, right));

float sampleDelta = 0.025;
float nrSamples = 0.0; 
for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
{
    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
    {
        
        vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
        vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

        irradiance += texture(u_CubeMap, sampleVec).rgb * cos(theta) * sin(theta);
        nrSamples++;
    }
}
irradiance = PI * irradiance * (1.0 / float(nrSamples));
  
    FragColor = vec4(irradiance, 1.0);
}