#version 450

layout (binding = 1) uniform samplerCube samplerEnv;

layout (set = 0, location = 0) in vec3 inUVW;

layout (set = 0, location = 0) out vec4 outColor;

void main() 
{
	outColor = vec4(textureLod(samplerEnv, inUVW, 0).rgb, 1.0);
}