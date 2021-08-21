#shader vertex
#version 460 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Bitangent;
layout(location = 4) in vec2 a_TexCoords;

struct ST_MeshData
{
    mat4 Matrix;
};

layout(std140, binding = 0) buffer shader_data
{
    ST_MeshData s_MeshData[];
};

uniform mat4 u_ViewProjectionMatrix;
uniform float u_MeshDataIndex;

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;

void main()
{
	int index = int(floor(u_MeshDataIndex + 0.1));
    mat4 Matrix = s_MeshData[index].Matrix;
	vec4 Pos = Matrix * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjectionMatrix * Pos;

	vec3 T = normalize(vec3(Matrix * vec4(a_Tangent, 0.0)));
	vec3 B = normalize(vec3(Matrix * vec4(a_Bitangent, 0.0)));
	vec3 N = normalize(vec3(Matrix * vec4(a_Normal, 0.0)));

	TBN = mat3(T, B, N);

	Position = vec3(Pos);
	Normal = normalize(mat3(Matrix) * a_Normal);
	TexCoord = a_TexCoords;	
}

#shader fragment
#version 460 core
out vec4 FragColor;

const float PI = 3.14159265359;
const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

struct ST_DirectionalLight 
{
    vec4 direction;
    vec4 radiance;
};

layout(std140, binding = 1) buffer LightData
{
	ST_DirectionalLight DirectionalLights[];
};

uniform int u_DirLightsCount;
uniform vec3 u_CameraPos;

uniform vec4 u_Color;
uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_CombinedMap;
uniform samplerCube u_CubemapSpecular;
uniform samplerCube u_CubemapIrradiance;
uniform sampler2D u_BRDF;

//multipliers
uniform float u_MetallicMult;
uniform float u_RoughnessMult;
uniform float u_AmbientLightMult;

//bools
uniform bool u_HasNormalMap;
uniform bool u_UseCombinedMap;

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
in mat3 TBN;


// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float ndfGGX(float cosLh, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(NdotV, k);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// ---------------------------------------------------------------------------------------------------
// The following code (from Unreal Engine 4's paper) shows how to filter the environment map
// for different roughnesses. This is mean to be computed offline and stored in cube map mips,
// so turning this on online will cause poor performance
float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, float Roughness, vec3 N)
{
	float a = Roughness * Roughness;
	float Phi = 2 * PI * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);
	vec3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;
	vec3 UpVector = abs(N.z) < 0.999 ? vec3(0, 0, 1) : vec3(1, 0, 0);
	vec3 TangentX = normalize(cross(UpVector, N));
	vec3 TangentY = cross(N, TangentX);
	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}

float TotalWeight = 0.0;

vec3 PrefilterEnvMap(float Roughness, vec3 R)
{
	vec3 N = R;
	vec3 V = R;
	vec3 PrefilteredColor = vec3(0.0);
	int NumSamples = 1024;
	for (int i = 0; i < NumSamples; i++)
	{
		vec2 Xi = Hammersley(i, NumSamples);
		vec3 H = ImportanceSampleGGX(Xi, Roughness, N);
		vec3 L = 2 * dot(V, H) * H - V;
		float NoL = clamp(dot(N, L), 0.0, 1.0);
		if (NoL > 0)
		{
			//PrefilteredColor += texture(u_EnvRadianceTex, L).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	return PrefilteredColor / TotalWeight;
}

// ---------------------------------------------------------------------------------------------------

vec3 RotateVectorAboutY(float angle, vec3 vec)
{
	angle = radians(angle);
	mat3x3 rotationMatrix = { vec3(cos(angle),0.0,sin(angle)),
							vec3(0.0,1.0,0.0),
							vec3(-sin(angle),0.0,cos(angle)) };
	return rotationMatrix * vec;
}
// ---------------------------------------------------------------------------------------------------

vec3 getNormalFromMap()
{
	vec3 normal = texture(u_Normal, TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);
	
	return normal;
}


void main()
{
	//vec4 Color = texture(u_Albedo, TexCoord) * u_Color;
	vec4 Color = u_Color;

	//CHECK THIS IF YOUR MESH IS GONE 
	if (Color.a < 0.1) discard;

	int DirectionalLightCount = u_DirLightsCount;

	// Sample input textures to get shading model params.
	vec3 albedo = pow(texture(u_Albedo, TexCoord).rgb, vec3(2.2)) * u_Color.rgb;
	vec3 normal = u_HasNormalMap ? getNormalFromMap() : Normal;
	float metalness = 0.0;
	float roughness = 0.0;
	if (u_UseCombinedMap)
	{
		vec3 combinedVec = pow(texture(u_CombinedMap, TexCoord).rgb, vec3(1.0 / 2.2)); // r = AO, g = roughness, b = metallic
		roughness = combinedVec.g * u_RoughnessMult;
		metalness = combinedVec.b * u_MetallicMult;
	}
	else
	{
		metalness = pow(texture(u_Metallic, TexCoord).r, 1.0 / 2.2) * u_MetallicMult;
		roughness = pow(texture(u_Roughness, TexCoord).r, 1.0 / 2.2) * u_RoughnessMult;
	}

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(u_CameraPos - Position);

	//normal
	vec3 N = normalize(normal);

	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));

	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, albedo, metalness);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
	for (int i = 0; i < u_DirLightsCount; ++i)
	{
		vec3 Li = -DirectionalLights[i].direction.xyz;
		vec3 Lradiance = DirectionalLights[i].radiance.rgb;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
		vec3 F = fresnelSchlickRoughness(F0, max(0.0, dot(Lh, Lo)), roughness);
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);
		specularBRDF = clamp(specularBRDF, vec3(0.0f), vec3(10.0f));
		// Total contribution for this light.
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	}

	// Ambient lighting (IBL).
	vec3 ambientLighting;
	{
		// Sample diffuse irradiance at normal direction.
		vec3 irradiance = texture(u_CubemapIrradiance, N).rgb;

		// Calculate Fresnel term for ambient lighting.
		// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
		// use cosLo instead of angle with light's half-vector (cosLh above).
		// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
		vec3 F = fresnelSchlickRoughness(F0, cosLo, roughness);

		// Get diffuse contribution factor (as with direct lighting).
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

		// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
		vec3 diffuseIBL = kd * albedo * irradiance;

		// Sample pre-filtered specular reflection environment at correct mipmap level.
		int specularTextureLevels = textureQueryLevels(u_CubemapSpecular);
		vec3 specularIrradiance = textureLod(u_CubemapSpecular, Lr, roughness * specularTextureLevels).rgb;

		// Split-sum approximation factors for Cook-Torrance specular BRDF.
		vec2 specularBRDF = texture(u_BRDF, vec2(cosLo, 1.0 - roughness)).rg;

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;


		// Total ambient lighting contribution.	
		ambientLighting = diffuseIBL + specularIBL;
		ambientLighting *= u_AmbientLightMult;
	}

	// Final fragment color.
	vec3 BaseColor = directLighting + ambientLighting;

	//tonemap
	const float gamma = 2.2;
	const float exposure = 1.0;
	const float pureWhite = 1.0;
	vec3 tonemappedColor = BaseColor * exposure;
	float luminance = dot(tonemappedColor, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (pureWhite * pureWhite))) / (1.0 + luminance);

	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * tonemappedColor;

	// Gamma correction.
	FragColor = vec4(pow(mappedColor, vec3(1.0 / gamma)), 1.0);
	//FragColor = vec4(BaseColor, 1.0);
}