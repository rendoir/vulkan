#version 450

layout (location = 0) in vec3 i_worldPosition;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv0;
layout (location = 3) in vec2 i_uv1;

layout (constant_id = 0) const float c_gamma = 2.2;
layout (constant_id = 1) const float c_exposure = 4.5;

// Camera set
layout (set = 0, binding = 0) uniform Camera
{
	mat4 projection;
	mat4 view;
	vec3 position;
} u_camera;

// Material set
layout (set = 2, binding = 0) uniform sampler2D u_albedoMap;
layout (set = 2, binding = 1) uniform sampler2D u_physicalMap;
layout (set = 2, binding = 2) uniform sampler2D u_normalMap;
layout (set = 2, binding = 3) uniform sampler2D u_aoMap;
layout (set = 2, binding = 4) uniform sampler2D u_emissiveMap;

// IBL set
layout (set = 3, binding = 0) uniform sampler2D u_brdflutMap;
layout (set = 3, binding = 1) uniform samplerCube u_irradianceCube;	
layout (set = 3, binding = 2) uniform samplerCube u_prefilteredCube;

// Lights set
struct Light
{
	vec3 color;
	vec3 position;
};

layout (set = 4, binding = 0) buffer Lights
{
	Light lights[128];
	int nrLights;
} u_lights;

layout (push_constant) uniform Material {
	vec4 baseColorFactor;
	vec3 emissiveFactor;
	int albedoTextureSet;
	int physicalTextureSet;
	int normalTextureSet;	
	int occlusionTextureSet;
	int emissiveTextureSet;
	float metallicFactor;	
	float roughnessFactor;	
	float alphaMask;
	float alphaMaskCutoff;
} pc_material;

layout (location = 0) out vec4 o_color;

#include "Common/Constants.glsl"
#include "Common/ColorSpace.glsl"

// Constants
const vec3 DIELECTRIC_F0 = vec3(0.04);

// Normal Mapping without Precomputed Tangents by Christian Schüler
vec3 GetNormalFromMap()
{
	vec2 inUV = pc_material.normalTextureSet == 0 ? i_uv0 : i_uv1;
	vec3 tangentNormal = texture(u_normalMap, inUV).xyz * 2.0 - 1.0;

	vec3 q1 = dFdx(i_worldPosition);
	vec3 q2 = dFdy(i_worldPosition);
	vec2 st1 = dFdx(inUV);
	vec2 st2 = dFdy(inUV);

	vec3 N = normalize(i_normal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

vec3 GetNormal()
{
	return (pc_material.normalTextureSet > -1) ? GetNormalFromMap() : normalize(i_normal);
}

void ApplyAmbientOcclusion(inout vec3 color)
{
	const float occlusionStrength = 1.0f;
	if (pc_material.occlusionTextureSet > -1) {
		float ao = texture(u_aoMap, (pc_material.occlusionTextureSet == 0 ? i_uv0 : i_uv1)).r;
		color = mix(color, color * ao, occlusionStrength);
	}
}

void ApplyEmissiveness(inout vec3 color)
{
	if (pc_material.emissiveTextureSet > -1) {
		vec3 emissive = SRGBtoLinear(texture(u_emissiveMap, pc_material.emissiveTextureSet == 0 ? i_uv0 : i_uv1)).rgb * pc_material.emissiveFactor;
		color += emissive;
	} else {	
		color += pc_material.emissiveFactor;	
	}
}

void GetAlbedo(out vec4 albedo)
{
	if (pc_material.albedoTextureSet > -1) {
		albedo = SRGBtoLinear(texture(u_albedoMap, pc_material.albedoTextureSet == 0 ? i_uv0 : i_uv1)) * pc_material.baseColorFactor;
	} else {
		albedo = pc_material.baseColorFactor;
	}

	if (albedo.a < pc_material.alphaMaskCutoff) {
		discard;
	}
}

void GetMetallicRoughness(out float metallic, out float roughness)
{
	metallic = pc_material.metallicFactor;
	roughness = pc_material.roughnessFactor;

	if (pc_material.physicalTextureSet > -1) {
		vec4 mrSample = texture(u_physicalMap, pc_material.physicalTextureSet == 0 ? i_uv0 : i_uv1);
		roughness = mrSample.g * roughness;
		metallic = mrSample.b * metallic;
	}
}

// Trowbridge-Reitz GGX Normal Distribution Function
float NormalDistributionFunction(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float aSquare = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotHSquare = NdotH * NdotH;

    float numerator = aSquare;
    float denominator = (NdotHSquare * (aSquare - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return numerator / max(denominator, EPSILON); // Prevent division by zero.
}

// Shlick Fresnel Function 
vec3 FresnelFunction(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Shlick Fresnel Function with Roughness	
vec3 FresnelRoughnessFunction(float cosTheta, vec3 F0, float roughness)
{	
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);	
}

// Schlick Smith GGX Geometry Function 
float GeometryFunction(vec3 N, vec3 V, vec3 L, float roughness)
{
	float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float gl = NdotL / (NdotL * (1.0 - k) + k);
    float gv = NdotV / (NdotV * (1.0 - k) + k);

    return gl * gv;
}

// Cook-Torrance Bidirectional Reflective Distribution Function
vec3 BRDF(vec3 L, vec3 V, vec3 N, vec3 radiance, vec4 albedo, float metallic, float roughness, vec3 F0)
{
	vec3 H = normalize (V + L);

	float D = NormalDistributionFunction(N, H, roughness);   
	float G = GeometryFunction(N, V, L, roughness);      
	vec3  F = FresnelFunction(max(dot(H, V), 0.0), F0);

	vec3 numerator = D * G * F; 
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0); 
	vec3 specular = numerator / max(denominator, EPSILON); // Prevent division by zero.
	
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;	  

	float NdotL = max(dot(N, L), 0.0);        

	vec3 color = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
	return color;
}

vec3 ComputeIBL(vec4 albedo, float metallic, float roughness, vec3 N, vec3 V, vec3 F0)
{	
    vec3 F = FresnelRoughnessFunction(max(dot(N, V), 0.0), F0, roughness);	
    	
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(u_irradianceCube, N).rgb;
    vec3 diffuse = irradiance * albedo.rgb;	
    	
	vec3 R = -normalize(reflect(V, N));	
    vec3 prefilteredColor = textureLod(u_prefilteredCube, R,  roughness * (float(textureQueryLevels(u_prefilteredCube)-1))).rgb;    	
    vec2 brdf = texture(u_brdflutMap, vec2(max(dot(N, V), 0.0), 1.0 - roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);	
    return kD * diffuse + specular;	
}

void main()
{
	vec4 albedo;
	float metallic;
	float roughness;

	GetAlbedo(albedo);
	GetMetallicRoughness(metallic, roughness);

	vec3 N = GetNormal();
	vec3 V = normalize(u_camera.position - i_worldPosition);

	vec3 F0 = mix(DIELECTRIC_F0, albedo.rgb, metallic);

	// Reflectance equation
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < u_lights.nrLights; i++) {
		vec3 uL = u_lights.lights[i].position - i_worldPosition;
		vec3 L = normalize(uL);
		float lightDistance = length(uL);
        float attenuation = 1.0 / (lightDistance * lightDistance);
		vec3 radiance = SRGBtoLinear(u_lights.lights[i].color) * attenuation;
		
		Lo += BRDF(L, V, N, radiance, albedo, metallic, roughness, F0);
	}
	
	ApplyEmissiveness(Lo);

	vec3 ambient = ComputeIBL(albedo, metallic, roughness, N, V, F0);	
	ApplyAmbientOcclusion(ambient);
	
	vec3 color = ambient + Lo;
	
	ApplyHDRTonemapping(color);
	ApplyGammaCorrection(color);
	
	o_color = vec4(color, albedo.a);
}
