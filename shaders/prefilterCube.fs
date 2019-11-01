#version 450

layout (location = 0) in vec3 inPos;
layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform samplerCube samplerEnv;

layout(push_constant) uniform PushConsts {
	layout (offset = 64) float roughness;
	layout (offset = 68) uint numSamples;
} consts;


// Constants
const float PI = 3.1415926535897932384626433832795;
const float epsilon = 1.175495e-38;

// Hammersley sequence
vec2 Hammersley(uint i, uint N) 
{
	uint bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = float(bits) * 2.3283064365386963e-10; // 0x100000000
	return vec2(float(i) /float(N), rdi);
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

// Normal Distribution function
float NormalDistributionFunction(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSquare = alpha * alpha;
	float denominator = dotNH * dotNH * (alphaSquare - 1.0) + 1.0;
	denominator = PI * denominator * denominator;
	
	return alphaSquare / max(denominator, epsilon); // Prevent division by zero.
}

vec3 prefilterEnvMap(vec3 R, float roughness)
{
	vec3 N = R;
	vec3 V = R;

	vec3 color = vec3(0.0);
	float totalWeight = 0.0;
	float envMapDim = float(textureSize(samplerEnv, 0).s);
	for(uint i = 0u; i < consts.numSamples; i++) {
		vec2 Xi = Hammersley(i, consts.numSamples);
		vec3 H = ImportanceSampleGGX(Xi, roughness, N);
		vec3 L = 2.0 * dot(V, H) * H - V;
		
		float dotNL = clamp(dot(N, L), 0.0, 1.0);
		if(dotNL > 0.0) {
			float dotNH = clamp(dot(N, H), 0.0, 1.0);
			float dotVH = clamp(dot(V, H), 0.0, 1.0);

			// Probability Distribution Function
			float pdf = NormalDistributionFunction(dotNH, roughness) * dotNH / (4.0 * dotVH) + epsilon;
			// Solid angle of the current sample
			float omegaS = 1.0 / (float(consts.numSamples) * pdf);
			// Solid angle of 1 pixel across all cube faces
			float omegaP = 4.0 * PI / (6.0 * envMapDim * envMapDim);
			
			float mipLevel = roughness == 0.0 ? 0.0 : clamp(0.5 * log2(omegaS / omegaP), 0.0f, float(textureQueryLevels(samplerEnv)));
			color += textureLod(samplerEnv, L, mipLevel).rgb * dotNL;
			totalWeight += dotNL;
		}
	}

	return (color / totalWeight);
}


void main()
{		
	vec3 N = normalize(inPos);
	outColor = vec4(prefilterEnvMap(N, consts.roughness), 1.0);
}
