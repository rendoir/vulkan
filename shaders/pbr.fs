#version 450

layout (location = 0) in vec3 inWorldPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV0;
layout (location = 3) in vec2 inUV1;

// Scene bindings

layout (set = 0, binding = 0) uniform UBO {
	mat4 projection;
	mat4 model;
	mat4 view;
} ubo;

// Material bindings

layout (set = 1, binding = 0) uniform sampler2D colorMap;
layout (set = 1, binding = 1) uniform sampler2D physicalDescriptorMap;
layout (set = 1, binding = 2) uniform sampler2D normalMap;
layout (set = 1, binding = 3) uniform sampler2D aoMap;
layout (set = 1, binding = 4) uniform sampler2D emissiveMap;

layout (push_constant) uniform Material {
	vec4 baseColorFactor;
	vec4 emissiveFactor;
	int colorTextureSet;
	int physicalTextureSet;
	int normalTextureSet;	
	int occlusionTextureSet;
	int emissiveTextureSet;
	float metallicFactor;	
	float roughnessFactor;	
	float alphaMask;
	float alphaMaskCutoff;
} material;

layout (location = 0) out vec4 outColor;


void main()
{
	vec4 baseColor;

	if (material.colorTextureSet > -1) {
		baseColor = texture(colorMap, material.colorTextureSet == 0 ? inUV0 : inUV1) * material.baseColorFactor;
    } else {
        baseColor = material.baseColorFactor;
    }

	outColor = baseColor;
}
