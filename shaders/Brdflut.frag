#version 450

layout (location = 0) in vec2 i_uv;
layout (location = 0) out vec4 o_color;
layout (constant_id = 0) const uint c_nrSamples = 1024u;

#include "Common/Constants.glsl"

// Hammersley sequence
vec2 Hammersley(uint i, uint N) 
{
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10; // 0x100000000
	return vec2(float(i)/float(N), rdi);
}

// Importance sampling GGX
vec3 ImportanceSampleGGX(vec2 Xi, float roughness, vec3 normal) 
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (alpha*alpha - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	vec3 halfway = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

	// Tangent space
	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = normalize(cross(normal, tangent));

	// Convert to world Space
	return normalize(tangent * halfway.x + bitangent * halfway.y + normal * halfway.z);
}

// Geometric Shadowing function
float GeometrySchlickGGX(float dotNL, float dotNV, float roughness)
{
	float k = (roughness * roughness) / 2.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

vec2 IntegrateBRDF(float dotNV, float roughness)
{
	// Normal always points along z-axis for the 2D lookup 
	const vec3 N = vec3(0.0, 0.0, 1.0);
	vec3 V = vec3(sqrt(1.0 - dotNV*dotNV), 0.0, dotNV);

	// For each sample, generates a sample vector that's biased
	// towards the preferred alignment direction (importance sampling)
	vec2 LUT = vec2(0.0);
	for(uint i = 0u; i < c_nrSamples; i++) {
		vec2 Xi = Hammersley(i, c_nrSamples);
		vec3 H = ImportanceSampleGGX(Xi, roughness, N);
		vec3 L = 2.0 * dot(V, H) * H - V;

		float dotNL = max(L.z, 0.0);
		float dotNH = max(H.z, 0.0);
		float dotVH = max(dot(V, H), 0.0); 

		if (dotNL > 0.0) {
			float G = GeometrySchlickGGX(dotNL, dotNV, roughness);
			float G_Vis = (G * dotVH) / (dotNH * dotNV);
			float Fc = pow(1.0 - dotVH, 5.0);
			LUT += vec2((1.0 - Fc) * G_Vis, Fc * G_Vis);
		}
	}
	return LUT / float(c_nrSamples);
}

void main() 
{
	o_color = vec4(IntegrateBRDF(i_uv.s, 1.0-i_uv.t), 0.0, 1.0);
}