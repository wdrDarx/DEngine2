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

//shadows
const int MaxDirectionalShadowMaps = 4;
uniform mat4 u_DirectionLightMatricies[MaxDirectionalShadowMaps];

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;
out vec4[MaxDirectionalShadowMaps] DirectionalLightProjectedVerticies;
out float DirLightClipSpacePosZ;

void main()
{
	int index = gl_InstanceID;
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

	//project directional light matricies
	for(int i = 0; i < MaxDirectionalShadowMaps; i++)
	{
		DirectionalLightProjectedVerticies[i] = u_DirectionLightMatricies[i] * Pos;
	}

	//DirLightClipSpacePosZ = vec3(u_ViewMatrix * Pos).z;
	DirLightClipSpacePosZ = gl_Position.z;
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
	vec4 LightSize;
};

layout(std140, binding = 1) buffer LightData
{
	ST_DirectionalLight DirectionalLights[];
};

uniform int u_DirLightsCount;
uniform vec3 u_CameraPos;

//material
uniform vec4 u_Color;
uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;
uniform sampler2D u_Metallic;
uniform sampler2D u_Roughness;
uniform sampler2D u_CombinedMap;
uniform samplerCube u_CubemapSpecular;
uniform samplerCube u_CubemapIrradiance;
uniform sampler2D u_BRDF;

//shadow maps
const int MaxDirectionalShadowMaps = 4;
uniform int u_DirectionalShadowMapCount;
uniform float u_CascadeEndClipSpace[MaxDirectionalShadowMaps];
uniform sampler2D u_DirectionalShadowMaps[MaxDirectionalShadowMaps];
in vec4 DirectionalLightProjectedVerticies[MaxDirectionalShadowMaps];
in float DirLightClipSpacePosZ;

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

vec3 getNormalFromMap()
{
	vec3 normal = texture(u_Normal, TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(TBN * normal);

	return normal;
}

float SampleShadowMap(sampler2D shadowMap, vec2 coords, float compare)
{	
	return step(compare, texture(shadowMap, coords).r);
}

float SampleShadowMapLinear(sampler2D shadowMap, vec2 coords, float compare, vec2 texelSize)
{
	vec2 pixelPos = coords/ texelSize + vec2(0.5);
	vec2 fracPart = fract(pixelPos);
	vec2 startTexel = (pixelPos - fracPart) * texelSize;

	float blTexel = SampleShadowMap(shadowMap, startTexel, compare);
	float brTexel = SampleShadowMap(shadowMap, startTexel + vec2(texelSize.x, 0.0), compare);
	float tlTexel = SampleShadowMap(shadowMap, startTexel + vec2(0.0, texelSize.y), compare);
	float trTexel = SampleShadowMap(shadowMap, startTexel + texelSize, compare);

	float mixA = mix(blTexel, tlTexel, fracPart.y);
	float mixB = mix(brTexel, trTexel, fracPart.y);

	return mix(mixA, mixB, fracPart.x);
}

/////////////////////////////////////////////
// PCSS
/////////////////////////////////////////////

// Penumbra

// this search area estimation comes from the following article: 
// http://developer.download.nvidia.com/whitepapers/2008/PCSS_Integration.pdf
float SearchWidth(float uvLightSize, float receiverDistance)
{
	const float NEAR = 0.01;
	return (1.0 / uvLightSize) * (receiverDistance - NEAR) / receiverDistance;
}


const vec2 PoissonDistribution[64] = vec2[](
	vec2(-0.884081, 0.124488),
	vec2(-0.714377, 0.027940),
	vec2(-0.747945, 0.227922),
	vec2(-0.939609, 0.243634),
	vec2(-0.985465, 0.045534),
	vec2(-0.861367, -0.136222),
	vec2(-0.881934, 0.396908),
	vec2(-0.466938, 0.014526),
	vec2(-0.558207, 0.212662),
	vec2(-0.578447, -0.095822),
	vec2(-0.740266, -0.095631),
	vec2(-0.751681, 0.472604),
	vec2(-0.553147, -0.243177),
	vec2(-0.674762, -0.330730),
	vec2(-0.402765, -0.122087),
	vec2(-0.319776, -0.312166),
	vec2(-0.413923, -0.439757),
	vec2(-0.979153, -0.201245),
	vec2(-0.865579, -0.288695),
	vec2(-0.243704, -0.186378),
	vec2(-0.294920, -0.055748),
	vec2(-0.604452, -0.544251),
	vec2(-0.418056, -0.587679),
	vec2(-0.549156, -0.415877),
	vec2(-0.238080, -0.611761),
	vec2(-0.267004, -0.459702),
	vec2(-0.100006, -0.229116),
	vec2(-0.101928, -0.380382),
	vec2(-0.681467, -0.700773),
	vec2(-0.763488, -0.543386),
	vec2(-0.549030, -0.750749),
	vec2(-0.809045, -0.408738),
	vec2(-0.388134, -0.773448),
	vec2(-0.429392, -0.894892),
	vec2(-0.131597, 0.065058),
	vec2(-0.275002, 0.102922),
	vec2(-0.106117, -0.068327),
	vec2(-0.294586, -0.891515),
	vec2(-0.629418, 0.379387),
	vec2(-0.407257, 0.339748),
	vec2(0.071650, -0.384284),
	vec2(0.022018, -0.263793),
	vec2(0.003879, -0.136073),
	vec2(-0.137533, -0.767844),
	vec2(-0.050874, -0.906068),
	vec2(0.114133, -0.070053),
	vec2(0.163314, -0.217231),
	vec2(-0.100262, -0.587992),
	vec2(-0.004942, 0.125368),
	vec2(0.035302, -0.619310),
	vec2(0.195646, -0.459022),
	vec2(0.303969, -0.346362),
	vec2(-0.678118, 0.685099),
	vec2(-0.628418, 0.507978),
	vec2(-0.508473, 0.458753),
	vec2(0.032134, -0.782030),
	vec2(0.122595, 0.280353),
	vec2(-0.043643, 0.312119),
	vec2(0.132993, 0.085170),
	vec2(-0.192106, 0.285848),
	vec2(0.183621, -0.713242),
	vec2(0.265220, -0.596716),
	vec2(-0.009628, -0.483058),
	vec2(-0.018516, 0.435703)
	);

const vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
	);

vec2 SamplePoisson(int index)
{
	return PoissonDistribution[index % 64];
}

float GetShadowBias(vec3 Normal, vec3 LightDirection)
{
	const float MIN_BIAS = 0.0004;
	return -max(MIN_BIAS * (1.0 - dot(Normal, LightDirection)), MIN_BIAS);

	//return -0.0004;
}

float FindBlockerDistance_DirectionalLight(vec3 Normal, vec3 LightDirection, vec3 ProjCoords, sampler2D shadowMap, float uvLightSize)
{
	float bias = GetShadowBias(Normal, LightDirection);
	int blockers = 0;
	float avgBlockerDistance = 0;
	int numBlockerSearchSamples = 64;
	float searchWidth = SearchWidth(uvLightSize, ProjCoords.z);
	for (int i = 0; i < numBlockerSearchSamples; i++)
	{
		float z = texture(shadowMap, ProjCoords.xy + SamplePoisson(i) * 1).r;
		if (z < (ProjCoords.z - bias))
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}
	if (blockers > 0)
		return avgBlockerDistance / blockers;
	else
		return -1;
}

float PCF_DirectionalLight(vec3 Normal, vec3 LightDirection, sampler2D shadowMap, vec3 ProjCoords, float uvRadius)
{
	float bias = GetShadowBias(Normal, LightDirection);
	int numPCFSamples = 64;

	float sum = 0;
	for (int i = 0; i < numPCFSamples; i++)
	{
		vec2 offset = SamplePoisson(i) * uvRadius;
		float z = texture(shadowMap, ProjCoords.xy + offset).r;
		sum += step(ProjCoords.z - bias, z);
	}
	return sum / numPCFSamples;
}

float PCSS_DirectionalLight(vec3 Normal, vec3 LightDirection, sampler2D shadowMap, vec3 ProjCoords, float uvLightSize)
{
	float blockerDistance = FindBlockerDistance_DirectionalLight(Normal, LightDirection, ProjCoords, shadowMap, uvLightSize);
	//if (blockerDistance == -1) // No occlusion
	//	return 1.0f;

	blockerDistance = 0.1;
	float penumbraWidth = (ProjCoords.z - blockerDistance) / blockerDistance;

	float NEAR = 0.01; // Should this value be tweakable?
	float uvRadius = penumbraWidth * uvLightSize * NEAR / ProjCoords.z; // Do we need to divide by ProjCoords.z?
	uvRadius = min(uvRadius, 0.002f);
	return PCF_DirectionalLight(Normal, LightDirection, shadowMap, ProjCoords, uvRadius);
}

float DirectionalLightShadowCalculation(int index, vec3 Normal, vec3 LightDirection, float LightUvSize)
{
	//vec4 fragPosLightSpace = DirLightMatrix * vec4(Position, 1.0);
	vec4 fragPosLightSpace = DirectionalLightProjectedVerticies[index];
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	//float bias = max(0.0000001 * (1.0 - dot(Normal, LightDirection)), 0.000001);

	//pcf
	const int Samples = 4;
	const int SamplesStart = (Samples - 1)/2;
	const int SamplesSquared = Samples * Samples;

	float shadow = 0.0;
	/*vec2 texelSize = 1.0 / textureSize(u_DirectionalShadowMaps[index], 0);
	for (int x = -Samples; x <= SamplesStart; ++x)
	{
		for (int y = -Samples; y <= SamplesStart; ++y)
		{
			vec2 coordsOffset = vec2(x, y) * texelSize;
			float pcfDepth = SampleShadowMapLinear(u_DirectionalShadowMaps[index], projCoords.xy + coordsOffset, currentDepth - bias, texelSize);
			shadow += pcfDepth;
		}
	}
	shadow /= SamplesSquared;*/

	shadow = PCSS_DirectionalLight(Normal, LightDirection, u_DirectionalShadowMaps[index], projCoords, LightUvSize / 1000);

	//shadow = step(projCoords.z, texture(u_DirectionalShadowMaps[index], projCoords.xy).r);
	return shadow;
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
		//vec2 specularBRDF = texture(u_BRDF, vec2(cosLo, 1.0 - roughness)).rg;
		vec2 specularBRDF = texture(u_BRDF, vec2(cosLo, roughness)).rg; // this is correct

		// Total specular IBL contribution.
		vec3 specularIBL = (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;


		// Total ambient lighting contribution.	
		ambientLighting = diffuseIBL + specularIBL;
		ambientLighting *= u_AmbientLightMult;
	}

	//directional shadows (start at 1.0 because its inverse)
	float ShadowSum = 0.0;
	int shadowCascade = 0;
	if(u_DirectionalShadowMapCount < 1)
	{ 
		ShadowSum = 1.0; //dont calculate shadows and leave shadow sum at 1.0 (default everything is lit)
	}
	else
	{ 				
		for (int i = 0; i < u_DirectionalShadowMapCount; i++)
		{
			if (DirLightClipSpacePosZ <= u_CascadeEndClipSpace[i])
			{ 
				shadowCascade = i;
				break;
			}
		}

		bool FadeCascades = true;
		if (FadeCascades)
		{
			float cascadeTransitionFade = 1000.0;

			float c0 = smoothstep(u_CascadeEndClipSpace[0] - cascadeTransitionFade * 0.5f, u_CascadeEndClipSpace[0] + cascadeTransitionFade * 0.5f, DirLightClipSpacePosZ);
			float c1 = smoothstep(u_CascadeEndClipSpace[1] - cascadeTransitionFade * 0.5f, u_CascadeEndClipSpace[1] + cascadeTransitionFade * 0.5f, DirLightClipSpacePosZ);
			float c2 = smoothstep(u_CascadeEndClipSpace[2] - cascadeTransitionFade * 0.5f, u_CascadeEndClipSpace[2] + cascadeTransitionFade * 0.5f, DirLightClipSpacePosZ);
			if (c0 > 0.0 && c0 < 1.0)
			{
				// Sample 0 & 1
				float shadowAmount0 = DirectionalLightShadowCalculation(0, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);
				float shadowAmount1 = DirectionalLightShadowCalculation(1, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);

				ShadowSum = mix(shadowAmount0, shadowAmount1, c0);
			}
			else if (c1 > 0.0 && c1 < 1.0)
			{
				// Sample 1 & 2
				float shadowAmount1 = DirectionalLightShadowCalculation(1, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);
				float shadowAmount2 = DirectionalLightShadowCalculation(2, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);

				ShadowSum = mix(shadowAmount1, shadowAmount2, c1);
			}
			else if (c2 > 0.0 && c2 < 1.0)
			{
				// Sample 2 & 3
				float shadowAmount2 = DirectionalLightShadowCalculation(2, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);
				float shadowAmount3 = DirectionalLightShadowCalculation(3, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);

				ShadowSum = mix(shadowAmount2, shadowAmount3, c2);
			}
			else
				ShadowSum = DirectionalLightShadowCalculation(shadowCascade, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);
		}
		else
			ShadowSum = DirectionalLightShadowCalculation(shadowCascade, normal, DirectionalLights[0].direction.xyz, DirectionalLights[0].LightSize.x);
	}
	//clamp the shadow sum 
	ShadowSum = clamp(ShadowSum, 0.0, 1.0);

	// Final fragment color.
	vec3 BaseColor = ShadowSum * directLighting + ambientLighting;

	vec3 CascadeColor = vec3(1.0);
	switch (shadowCascade)
	{
	case 0:
		CascadeColor.rgb *= vec3(1.0f, 0.25f, 0.25f);
		break;
	case 1:
		CascadeColor.rgb *= vec3(0.25f, 1.0f, 0.25f);
		break;
	case 2:
		CascadeColor.rgb *= vec3(0.25f, 0.25f, 1.0f);
		break;
	case 3:
		CascadeColor.rgb *= vec3(1.0f, 1.0f, 0.25f);
		break;
	}

	//preview cascades
	//BaseColor *= CascadeColor;

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